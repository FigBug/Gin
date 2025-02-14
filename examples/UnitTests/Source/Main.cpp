#include <JuceHeader.h>

int main (int argc, char* argv[])
{
    juce::UnitTestRunner runner;

    if (argc == 1)
    {
        printf ("Running all unit tests\n");
        runner.runAllTests ();
    }
    else
    {
        for (auto i = 1; i < argc; i++)
        {
            printf ("Running unit test: %s\n", juce::String (argv[i]).toRawUTF8());
            runner.runTestsInCategory (juce::String (argv[i]));
        }
    }

    int numFailures = 0;

    for (auto i = 0; i < runner.getNumResults(); i++)
        if (auto res = runner.getResult (i))
            numFailures += res->failures;

    juce::DeletedAtShutdown::deleteAll();

    printf ("Unit tests complete: %d errors\n", numFailures);

    if (numFailures > 0)
        return 1;

    return 0;
}
