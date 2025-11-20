//==============================================================================
#if GIN_UNIT_TESTS

class ProgramTests : public juce::UnitTest
{
public:
    ProgramTests() : juce::UnitTest ("Program", "gin_plugin") {}

    void runTest() override
    {
        testBasicProgramCreation();
        testProgramMetadata();
        testParameterStorage();
        testProgramState();
        testFilePaths();
    }

private:
    void testBasicProgramCreation()
    {
        beginTest ("Basic Program Creation");

        gin::Program program;

        expect (program.name.isEmpty(), "Name should be empty by default");
        expect (program.author.isEmpty(), "Author should be empty by default");
        expect (program.tags.isEmpty(), "Tags should be empty by default");
        expect (!program.fullyLoaded, "Should not be fully loaded by default");
        expect (program.parameters.isEmpty(), "Parameters should be empty by default");
    }

    void testProgramMetadata()
    {
        beginTest ("Program Metadata");

        gin::Program program;

        program.name = "Test Preset";
        program.author = "Test Author";
        program.tags.add ("Bass");
        program.tags.add ("Dark");
        program.tags.add ("Synth");

        expect (program.name == "Test Preset", "Name should be set");
        expect (program.author == "Test Author", "Author should be set");
        expect (program.tags.size() == 3, "Should have 3 tags");
        expect (program.tags.contains ("Bass"), "Should contain Bass tag");
        expect (program.tags.contains ("Dark"), "Should contain Dark tag");
        expect (program.tags.contains ("Synth"), "Should contain Synth tag");
    }

    void testParameterStorage()
    {
        beginTest ("Parameter Storage");

        gin::Program program;

        // Add some parameter states
        gin::Parameter::ParamState state1;
        state1.uid = "param1";
        state1.value = 0.5f;

        gin::Parameter::ParamState state2;
        state2.uid = "param2";
        state2.value = 0.75f;

        program.parameters.add (state1);
        program.parameters.add (state2);

        expect (program.parameters.size() == 2, "Should have 2 parameters");
        expect (program.parameters[0].uid == "param1", "First param UID should match");
        expectWithinAbsoluteError (program.parameters[0].value, 0.5f, 0.001f, "First param value should match");
        expect (program.parameters[1].uid == "param2", "Second param UID should match");
        expectWithinAbsoluteError (program.parameters[1].value, 0.75f, 0.001f, "Second param value should match");
    }

    void testProgramState()
    {
        beginTest ("Program State");

        gin::Program program;

        // Create a ValueTree state
        juce::ValueTree state ("ProgramState");
        state.setProperty ("customValue", 42, nullptr);
        state.setProperty ("customString", "test", nullptr);

        program.state = state;

        expect (program.state.isValid(), "State should be valid");
        expect (program.state.getType() == juce::Identifier ("ProgramState"), "State type should match");
        expect (int (program.state.getProperty ("customValue")) == 42, "Custom value should be preserved");
        expect (program.state.getProperty ("customString").toString() == "test", "Custom string should be preserved");
    }

    void testFilePaths()
    {
        beginTest ("File Paths");

        gin::Program program;
        program.name = "TestPreset";

        // Create a temporary directory
        auto tempDir = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("GinTestPrograms");
        tempDir.createDirectory();

        auto presetFile = program.getPresetFile (tempDir);
        expect (presetFile.exists() == false, "Preset file should not exist yet");
        expect (presetFile.getParentDirectory() == tempDir, "Parent directory should match");
        expect (presetFile.getFileNameWithoutExtension().isNotEmpty(), "Should have a filename");

        // Clean up
        tempDir.deleteRecursively();
    }
};

static ProgramTests programTests;

#endif
