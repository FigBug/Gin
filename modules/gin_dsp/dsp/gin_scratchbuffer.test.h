/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ScratchBufferTests : public juce::UnitTest
{
public:
    ScratchBufferTests()
        : UnitTest ("ScratchBuffer class", "gin_dsp")
    {
    }

    void runTest() override
    {
        beginTest ("ref count 1");
        {
            auto buf = std::make_unique<ScratchBuffer> (2, 1024);
            expect (BufferCache::getInstance()->getUsedBuffers() == 1);
            buf = nullptr;
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("ref count 2");
        {
            auto buf1 = std::make_unique<ScratchBuffer> (2, 1024);
            expect (BufferCache::getInstance()->getUsedBuffers() == 1);

            auto buf2 = std::make_unique<ScratchBuffer> (*buf1);
            expect (BufferCache::getInstance()->getUsedBuffers() == 2);

            buf1 = nullptr;
            buf2 = nullptr;
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("ref count 3");
        {
            juce::AudioSampleBuffer source (2, 1024);

            auto buf1 = std::make_unique<ScratchBuffer> (source);
            expect (BufferCache::getInstance()->getUsedBuffers() == 1);

            auto buf2 = std::make_unique<ScratchBuffer> (source);
            expect (BufferCache::getInstance()->getUsedBuffers() == 2);

            buf1 = nullptr;
            buf2 = nullptr;
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("mono buffer from stereo");
        {
            juce::AudioSampleBuffer source (2, 1024);
            {
                auto mono = monoBuffer (source);
                expect (mono.getNumChannels() == 1);
                expect (BufferCache::getInstance()->getUsedBuffers() == 1);
            }
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("mono buffer from mono");
        {
            juce::AudioSampleBuffer source (1, 1024);
            for (int i = 0; i < 1024; i++)
                source.setSample (0, i, (float) i);

            {
                auto mono = monoBuffer (source);
                expect (mono.getNumChannels() == 1);
                expect (mono.getNumSamples() == 1024);
                expect (BufferCache::getInstance()->getUsedBuffers() == 1);

                for (int i = 0; i < 1024; i++)
                    expect (mono.getSample (0, i) == (float) i);
            }
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("sizes");
        {
            juce::Random r ( 0 );

            for (auto i = 0; i < 5000; i++)
            {
                int chans = r.nextInt ({1,5});
                int samps = r.nextInt ({32,88200});

                juce::AudioSampleBuffer buf (chans, samps);
                expect (buf.getNumChannels() == chans);
                expect (buf.getNumSamples() == samps);
            }
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }
    }
};

static ScratchBufferTests scratchBufferTests;

#endif
