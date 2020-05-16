/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================

std::unique_ptr<Sample> Sample::fromFile (const File& f)
{
    SharedResourcePointer<AudioFormatManager> formats;
    formats->registerBasicFormats();
        
    if (auto reader = std::unique_ptr<AudioFormatReader> (formats->createReaderFor (f)))
    {
        auto sample = std::make_unique<Sample>();
        
        sample->sampleRate = reader->sampleRate;
        sample->length = int (reader->lengthInSamples);
        sample->inMarker = 0;
        sample->outMarker = sample->length - 1;
        
        sample->data.setSize (int (reader->numChannels), sample->length);
        
        reader->read (&sample->data, 0, sample->length, 0, true, true);
        
        sample->l = sample->data.getReadPointer (0);
        sample->r = sample->data.getReadPointer (reader->numChannels == 1 ? 0 : 1);
        
        return sample;
    }
    
    return {};
}

void Sample::read (AudioSampleBuffer& buffer, int samplePos, int bufferPos, int len)
{
    if (len == -1)
        len = buffer.getNumSamples() - bufferPos;
    
    int readPos = inMarker + samplePos;
    int readAmt = jmin (len, outMarker + 1 - readPos);
    
    for (int ch = 0; ch < buffer.getNumChannels(); ch++)
        buffer.copyFrom (ch, bufferPos, data, jmin (ch, data.getNumChannels() - 1), readPos, readAmt);
    
    if (readAmt < len)
        buffer.clear (bufferPos + readAmt, len - readAmt);
    
}
