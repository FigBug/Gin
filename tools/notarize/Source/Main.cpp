/*
  =============================================================================

    This file was auto-generated

    It contains the basic startup code for a JUCE application

  =============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

//=============================================================================
String execute (String cmd, bool verbose)
{
	ChildProcess cp;

    if (verbose)
        printf ("cmd: %s\n", cmd.toRawUTF8());

	auto tokens = StringArray::fromTokens (cmd, true);
	StringArray params;

	for (auto t : tokens)
		params.add (t.unquoted());

	if (cp.start (params))
	{
		while (! cp.waitForProcessToFinish (100))
            Thread::sleep (100);

		auto output = cp.readAllProcessOutput();

        if (verbose)
            printf ("output:\n%s\n", output.toRawUTF8());

        return output;
	}

    printf ("error: failed to launch\n");
    return {};
}

String parseRequestUuid (String xml)
{
    auto plist = gin::parsePlist (xml);
    var obj = plist.getProperty ("notarization-upload", "");
    if (obj.isObject())
    {
        return obj.getProperty ("RequestUUID", "");
    }
    return "";
}

String parseStatus (String xml)
{
    auto plist = gin::parsePlist (xml);
    auto info = plist.getProperty ("notarization-info", "");
    if (! info.isVoid())
        return info.getProperty ("Status", "");
    
	return {};
}

String parseStatusMessage (String xml)
{
    auto plist = gin::parsePlist (xml);
    auto info = plist.getProperty ("notarization-info", "");
    if (! info.isVoid())
        return info.getProperty ("Status Message", "");

    return {};
}

String parseBuildLog (String xml)
{
    auto plist = gin::parsePlist (xml);
    auto info = plist.getProperty ("notarization-info", "");
    if (! info.isVoid())
        return info.getProperty ("LogFileURL", "");

    return {};
}

String parseBundleId (File f)
{
    auto plist = gin::parsePlist (f);
    return plist.getProperty ("CFBundleIdentifier", "");
}

//==============================================================================
int main (int argc, char* argv[])
{
    auto ver = Time::getCompilationDate().toString (true, true);

    printf ("Notarize: version: %s\n", ver.toRawUTF8());
    printf ("argc %d\n", argc);

	if (argc < 4)
	{
		printf ("Usage: notarize [-v] [-ns] PATH USERNAME PASSWORD [BUNDLE_ID]\n");
        return 0;
	}

    bool verbose = false;
    bool staple = true;

    StringArray args;
    for (int i = 0; i < argc; i++)
    {
        auto arg = String (argv[i]);
        if (arg.startsWith ("-"))
        {
            if (arg == "-v") verbose = true;
            if (arg == "-ns") staple = false;
        }
        else
        {
            args.add (argv[i]);
        }
    }

	File path = File::getCurrentWorkingDirectory().getChildFile (args[1]);
    if (! path.existsAsFile() && ! path.isDirectory())
    {
        printf ("\"%s\" not found\n", path.getFullPathName().toRawUTF8());
        return 1;
    }

    path.getParentDirectory().setAsCurrentWorkingDirectory();
    
    auto cwd = path.getParentDirectory().getFullPathName();
    printf ("Current working dir: %s\n", cwd.toRawUTF8());

	String username = args[2];
	String password = args[3];
    String bundleId = args[4];
    
    if (bundleId.isEmpty())
        bundleId = parseBundleId (path.getChildFile ("Contents/Info.plist"));
    
    if (bundleId.isEmpty())
    {
        printf ("Can't find bundle id\n");
        return 1;
    }

	File notarizePath;

	if (path.isDirectory())
	{
		// If it's a bundle, zip it up
		notarizePath = path.withFileExtension (".zip");

        {
            ZipFile::Builder builder;
            builder.addFile (path, 9);
        }

        notarizePath.deleteFile ();
        execute ("ditto -c -k --keepParent " + path.getFullPathName().quoted() + " " + notarizePath.getFullPathName().quoted(), verbose);
	}
	else
	{
		// If it's a pkg, just use as is
		notarizePath = path;
	}

	// Upload to notarize service
	String uuid;
	{
        printf ("Uploading to notarization service\n");
        auto output = execute ("xcrun altool --notarize-app --primary-bundle-id " + bundleId.quoted () + " --username " +
                               username.quoted () + " --password " + password.quoted() + " --file " +
                               notarizePath.getFullPathName().quoted() + " --output-format xml", verbose);

        if (verbose)
            printf ("%s\n", output.toRawUTF8());

		uuid = parseRequestUuid (output);
        
        if (path.isDirectory())
            notarizePath.deleteFile();

		if (uuid.isEmpty())
		{
			printf ("Notarize upload failed %s\n", output.toRawUTF8());
			return 1;
		}
        else
        {
            printf ("Notarize uuid: %s\n", uuid.toRawUTF8());
        }
	}

	// Wait for notarize to finish
	int tries = 0;
	while (true)
	{
		auto output = execute ("xcrun altool --notarization-info " + uuid.quoted() + " --username " + username.quoted() +
                               " --password " + password.quoted() + " --output-format xml", verbose);

        if (verbose)
            printf ("%s\n", output.toRawUTF8());

		auto status = parseStatus (output);
		if (status == "success")
        {
            printf ("Notarize success\n");
			break;
        }

        if (status == "invalid")
        {
            auto statusMessage = parseStatusMessage (output);
            auto buildLog = parseBuildLog (output);

            if (! statusMessage.contains ("UUID"))
            {
                printf ("Notarize failed: %s\n", statusMessage.toRawUTF8());
                printf ("Notarize log: %s\n", buildLog.toRawUTF8());

                return 1;
            }
        }

        tries++;
        
        printf ("Checking status: %d of 500\n", tries);
        
        if (tries == 500)
        {
			printf ("Notarize failed: %s\n", status.toRawUTF8());
			return 1;
		}

		Thread::sleep (500);
	}

	// Staple and verify
    if (staple)
	{
        execute ("xcrun stapler staple " + path.getFullPathName().quoted(), verbose);
        auto output = execute ("xcrun stapler validate " + path.getFullPathName().quoted(), verbose);

        if (output.contains ("The validate action worked!"))
        {
            printf ("Staple success\n");
            return 0;
        }
        
        printf ("Staple failed\n");
        return 1;
	}
}

