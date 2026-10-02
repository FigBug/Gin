/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Adds parameter and program commands for an AudioProcessor to a RemoteServer.

    Commands added: processor, params, getParam, setParam, setProgram.
    Parameters are addressed by paramID, name or index.

    The processor must outlive the server, or you must call removeCommand for
    each of these before it is destroyed.

    Only available when juce_audio_processors is part of the project.
*/
inline void addAudioProcessorCommands (RemoteServer& server, juce::AudioProcessor& processor)
{
    using Result = RemoteServer::CommandResult;

    auto findParam = [&processor] (const juce::var& key) -> juce::AudioProcessorParameter*
    {
        auto& params = processor.getParameters();

        if (key.isInt() || key.isInt64() || key.isDouble())
        {
            int idx = int (key);
            return juce::isPositiveAndBelow (idx, params.size()) ? params[idx] : nullptr;
        }

        auto s = key.toString();

        for (auto p : params)
            if (auto withId = dynamic_cast<juce::AudioProcessorParameterWithID*> (p))
                if (withId->paramID == s)
                    return p;

        for (auto p : params)
            if (p->getName (256) == s)
                return p;

        for (auto p : params)
            if (p->getName (256).equalsIgnoreCase (s))
                return p;

        if (s.containsOnly ("0123456789") && s.isNotEmpty())
        {
            int idx = s.getIntValue();
            return juce::isPositiveAndBelow (idx, params.size()) ? params[idx] : nullptr;
        }

        return nullptr;
    };

    auto describeParam = [] (juce::AudioProcessorParameter& p)
    {
        auto obj = new juce::DynamicObject();
        obj->setProperty ("index", p.getParameterIndex());
        if (auto withId = dynamic_cast<juce::AudioProcessorParameterWithID*> (&p))
            obj->setProperty ("id", withId->paramID);
        obj->setProperty ("name", p.getName (256));
        obj->setProperty ("value", p.getValue());
        obj->setProperty ("text", p.getCurrentValueAsText());
        obj->setProperty ("default", p.getDefaultValue());
        if (p.getLabel().isNotEmpty())
            obj->setProperty ("label", p.getLabel());
        if (p.isDiscrete())
            obj->setProperty ("steps", p.getNumSteps());
        if (p.isBoolean())
            obj->setProperty ("boolean", true);
        if (! p.isAutomatable())
            obj->setProperty ("automatable", false);
        return juce::var (obj);
    };

    server.addCommand ("processor", "Audio processor info: name, sample rate, buses, programs", "{}",
                       [&processor] (const juce::var&)
    {
        auto obj = new juce::DynamicObject();
        obj->setProperty ("name", processor.getName());
        obj->setProperty ("sampleRate", processor.getSampleRate());
        obj->setProperty ("blockSize", processor.getBlockSize());
        obj->setProperty ("latency", processor.getLatencySamples());
        obj->setProperty ("inputChannels", processor.getTotalNumInputChannels());
        obj->setProperty ("outputChannels", processor.getTotalNumOutputChannels());
        obj->setProperty ("parameterCount", processor.getParameters().size());
        obj->setProperty ("currentProgram", processor.getCurrentProgram());
        obj->setProperty ("hasEditor", processor.hasEditor());
        obj->setProperty ("editorOpen", processor.getActiveEditor() != nullptr);

        juce::Array<juce::var> programs;
        for (int i = 0; i < processor.getNumPrograms(); i++)
            programs.add (processor.getProgramName (i));
        obj->setProperty ("programs", programs);

        return Result (juce::var (obj));
    });

    server.addCommand ("params", "List parameters with normalised values and display text",
                       R"({"filter":"string: only params whose id or name contains this"})",
                       [&processor, describeParam] (const juce::var& args)
    {
        auto filter = RemoteServer::getArg (args, "filter").toString().toLowerCase();

        juce::Array<juce::var> list;
        for (auto p : processor.getParameters())
        {
            if (filter.isNotEmpty())
            {
                auto id = dynamic_cast<juce::AudioProcessorParameterWithID*> (p) != nullptr ? dynamic_cast<juce::AudioProcessorParameterWithID*> (p)->paramID : juce::String();
                if (! id.toLowerCase().contains (filter) && ! p->getName (256).toLowerCase().contains (filter))
                    continue;
            }
            list.add (describeParam (*p));
        }
        return Result (list);
    });

    server.addCommand ("getParam", "Read one parameter",
                       R"({"param":"any!: paramID, name or index"})",
                       [findParam, describeParam] (const juce::var& args)
    {
        auto key = RemoteServer::getArg (args, "param");
        auto p = findParam (key);
        if (p == nullptr)
            return Result::fail ("No parameter '" + key.toString() + "'");
        return Result (describeParam (*p));
    });

    server.addCommand ("setParam", "Set one parameter, notifying the host",
                       R"({"param":"any!: paramID, name or index","value":"any!: normalised 0..1 number, or display text as a string","gesture":"bool: wrap in begin/endChangeGesture, default true"})",
                       [findParam, describeParam] (const juce::var& args)
    {
        auto key = RemoteServer::getArg (args, "param");
        auto p = findParam (key);
        if (p == nullptr)
            return Result::fail ("No parameter '" + key.toString() + "'");

        auto value = RemoteServer::getArg (args, "value");
        float normalised = value.isString() ? p->getValueForText (value.toString()) : float (double (value));
        normalised = juce::jlimit (0.0f, 1.0f, normalised);

        bool gesture = bool (RemoteServer::getArg (args, "gesture", true));
        if (gesture) p->beginChangeGesture();
        p->setValueNotifyingHost (normalised);
        if (gesture) p->endChangeGesture();

        return Result (describeParam (*p));
    });

    server.addCommand ("setProgram", "Select a program by index or name",
                       R"({"program":"any!: index or name"})",
                       [&processor] (const juce::var& args)
    {
        auto key = RemoteServer::getArg (args, "program");
        int idx = -1;

        if (key.isString())
        {
            for (int i = 0; i < processor.getNumPrograms(); i++)
                if (processor.getProgramName (i) == key.toString())
                    idx = i;
        }
        else
        {
            idx = int (key);
        }

        if (! juce::isPositiveAndBelow (idx, processor.getNumPrograms()))
            return Result::fail ("No program '" + key.toString() + "'");

        processor.setCurrentProgram (idx);
        return Result (processor.getProgramName (idx));
    });
}
