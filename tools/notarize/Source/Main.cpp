/*
  =============================================================================

    This file was auto-generated

    It contains the basic startup code for a JUCE application

  =============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

//=============================================================================
String execute (String cmd)
{
	ChildProcess cp;

	auto tokens = StringArray::fromTokens (cmd, true);
	StringArray params;

	for (auto t : tokens)
		params.add (t.unquoted());

	if (cp.start (params))
	{
		while (! cp.waitForProcessToFinish (100))
            Thread::sleep (100);

		return cp.readAllProcessOutput();
	}
    return {};
}

String parseRequestUuid (String xml)
{
    auto plist = gin::parsePlist (xml);
    return plist.getProperty ("notarization-upload", "");
}

String parseStatus (String xml)
{
    auto plist = gin::parsePlist (xml);
    auto info = plist.getProperty ("notarization-info", "");
    if (! info.isVoid())
        return info.getProperty ("Status", "");
    
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
	if (argc != 4 && argc != 5)
	{
		printf ("Usage: notarize PATH USERNAME PASSWORD [BUNDLE_ID]\n");
        return 0;
	}

	File path = File::getCurrentWorkingDirectory().getChildFile (argv[1]);
    if (! path.existsAsFile() && ! path.isDirectory())
    {
        printf ("\"%s\" not found\n", path.getFullPathName().toRawUTF8());
        return 1;
    }

    path.getParentDirectory().setAsCurrentWorkingDirectory();

	String username = argv[2];
	String password = argv[3];
    String bundleId = (argc == 5) ? argv[4] : "";
    
    if (bundleId.isEmpty())
        bundleId = parseBundleId (path);
    
    if (bundleId.isEmpty())
    {
        printf ("Can't find bundle id\n");
        return 1;
    }

	File notarizePath;

	if (path.isDirectory())
	{
		// If it's a bundle, zip it up
		notarizePath = path.withFileExtension(".zip");

        {
            ZipFile::Builder builder;
            builder.addFile (path, 9);
        }

        notarizePath.deleteFile ();
        execute ("ditto -c -k --keepParent " + path.getFullPathName().quoted() + " " + notarizePath.getFullPathName().quoted());
	}
	else
	{
		// If it's a pkg, just use as is
		notarizePath = path;
	}

	// Upload to notarize service
	String uuid;
	{
        auto output = execute ("xcrun altool --notarize-app --primary-bundle-id " + bundleId.quoted () + " --username " +
                               username.quoted () + " --password " + password.quoted() + " --file " +
                               notarizePath.getFullPathName().quoted() + " --output-format xml" );

		uuid = parseRequestUuid (output);
        
        if (path.isDirectory())
            notarizePath.deleteFile();

		if (uuid.isEmpty())
		{
			printf ("Notarize upload failed\n");
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
                               " --password " + password.quoted() + " --output-format xml");

		auto status = parseStatus (output);
		if (status == "success")
			break;

        tries++;
        
        printf ("Attempt %d of 500\n", tries);
        
        if (tries == 500)
        {
			printf ("Notarize failed: %s\n", status.toRawUTF8());
			return 1;
		}

		Thread::sleep (500);
	}

	// Staple and verify
	{
        execute ("xcrun stapler staple " + path.getFullPathName().quoted());
        auto output = execute ("xcrun stapler validate " + path.getFullPathName().quoted());
                 
        return output.contains ("The validate action worked!");
	}
}

