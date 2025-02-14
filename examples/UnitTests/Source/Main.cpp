#include <JuceHeader.h>

int main (int argc, char* argv[])
{
    juce::UnitTestRunner runner;

    if (argc == 1)
    {
        runner.runAllTests ();
    }
    else
    {
        for (auto i = 1; i < argc; i++)
            runner.runTestsInCategory (juce::String (argv[i]));
    }

    int numFailures = 0;

    for (auto i = 0; i < runner.getNumResults(); i++)
        if (auto res = runner.getResult (i))
            numFailures += res->failures;

    juce::DeletedAtShutdown::deleteAll();

    if (numFailures > 0)
        return 1;

    return 0;
}
