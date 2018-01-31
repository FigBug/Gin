/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/
using namespace jpeglibNamespace;
using namespace pnglibNamespace;

//==============================================================================
#define JPEG_RST0    0xD0    /* RST0 marker code */
#define JPEG_EOI     0xD9    /* EOI marker code */
#define JPEG_APP0    0xE0    /* APP0 marker code */
#define JPEG_COM     0xFE    /* COM marker code */

static void dummyCallback1 (j_decompress_ptr) {}

static void jpegSkip (j_decompress_ptr decompStruct, long num)
{
    decompStruct->src->next_input_byte += num;
    
    num = jmin (num, (long) decompStruct->src->bytes_in_buffer);
    decompStruct->src->bytes_in_buffer -= (size_t) num;
}

static boolean jpegFill (j_decompress_ptr)
{
    return 0;
}

static void fatalErrorHandler (j_common_ptr p)          { *((bool*) (p->client_data)) = true; }
static void silentErrorCallback1 (j_common_ptr)         {}
static void silentErrorCallback2 (j_common_ptr, int)    {}
static void silentErrorCallback3 (j_common_ptr, char*)  {}

static void setupSilentErrorHandler (struct jpeg_error_mgr& err)
{
    zerostruct (err);
    
    err.error_exit      = fatalErrorHandler;
    err.emit_message    = silentErrorCallback2;
    err.output_message  = silentErrorCallback1;
    err.format_message  = silentErrorCallback3;
    err.reset_error_mgr = silentErrorCallback1;
}

bool loadJPEGMetadataFromStream (OwnedArray<ImageMetadata>& metadata, InputStream& input)
{
    MemoryBlock mb;
    input.readIntoMemoryBlock (mb);
    
    if (mb.getSize() > 16)
    {
        struct jpeg_decompress_struct jpegDecompStruct;
        
        struct jpeg_error_mgr jerr;
        setupSilentErrorHandler (jerr);
        jpegDecompStruct.err = &jerr;
        
        jpeg_create_decompress (&jpegDecompStruct);
        
        jpeg_save_markers (&jpegDecompStruct, JPEG_COM, 0xFFFF);
        for (int m = 0; m < 16; m++)
            jpeg_save_markers (&jpegDecompStruct, JPEG_APP0 + m, 0xFFFF);
            
        jpegDecompStruct.src = (jpeg_source_mgr*)(jpegDecompStruct.mem->alloc_small)((j_common_ptr)(&jpegDecompStruct), JPOOL_PERMANENT, sizeof (jpeg_source_mgr));
        
        jpegDecompStruct.src->init_source       = dummyCallback1;
        jpegDecompStruct.src->fill_input_buffer = jpegFill;
        jpegDecompStruct.src->skip_input_data   = jpegSkip;
        jpegDecompStruct.src->resync_to_restart = jpeg_resync_to_restart;
        jpegDecompStruct.src->term_source       = dummyCallback1;
        
        jpegDecompStruct.src->next_input_byte   = (const unsigned char*) mb.getData();
        jpegDecompStruct.src->bytes_in_buffer   = mb.getSize();
        
        jpeg_read_header (&jpegDecompStruct, TRUE);
        
        jpeg_saved_marker_ptr marker = jpegDecompStruct.marker_list;
        while (marker)
        {
            ImageMetadata* md;
            if (marker->marker == JPEG_COM && (md = CommentMetadata::create (marker->data, int (marker->data_length))) != nullptr)
                metadata.add (md);
            if (marker->marker == JPEG_APP0 + 1 && (md = ExifMetadata::create (marker->data, int (marker->data_length))) != nullptr)
                metadata.add (md);
            if (marker->marker == JPEG_APP0 + 1 && (md = XmpMetadata::createFromJpg (marker->data, int (marker->data_length))) != nullptr)
                metadata.add (md);
            if (marker->marker == JPEG_APP0 + 13 && (md = IptcMetadata::create (marker->data, int (marker->data_length))) != nullptr)
                metadata.add (md);
            
            marker = marker->next;
        }

    }
    return metadata.size() > 0;
}

//==============================================================================
static void pngReadCallback (png_structp pngReadStruct, png_bytep data, png_size_t length)
{
    InputStream* const in = (InputStream*) png_get_io_ptr (pngReadStruct);
    in->read (data, (int) length);
}

//==============================================================================
bool loadPNGMetadataFromStream (OwnedArray<ImageMetadata>& metadata, InputStream& in)
{
    Image* image = 0;
    
    png_structp pngReadStruct;
    png_infop pngInfoStruct;
    
    pngReadStruct = png_create_read_struct (PNG_LIBPNG_VER_STRING, 0, 0, 0);
    
    if (pngReadStruct != 0)
    {
        pngInfoStruct = png_create_info_struct (pngReadStruct);
        
        if (pngInfoStruct == 0)
        {
            png_destroy_read_struct (&pngReadStruct, 0, 0);
            return 0;
        }
        
        // read the header..
        png_set_read_fn (pngReadStruct, &in, pngReadCallback);
        png_read_info (pngReadStruct, pngInfoStruct);
        
        for (int i = 0; i < pngInfoStruct->num_text; i++)
        {
            if (! strcmp (pngInfoStruct->text[i].key, "XML:com.adobe.xmp"))
            {
                ImageMetadata* md = XmpMetadata::createFromPng (pngInfoStruct->text[i].text, jmax((int)pngInfoStruct->text[i].text_length, (int)pngInfoStruct->text[i].itxt_length));
                metadata.add (md);
            }
        }
        png_destroy_read_struct (&pngReadStruct, &pngInfoStruct, 0);
    }
    
    return image;
}

//==============================================================================
ImageMetadata::ImageMetadata(const String& type_) : type(type_)
{
}

ImageMetadata::~ImageMetadata()
{
}

bool ImageMetadata::getFromImage (InputStream& is, OwnedArray<ImageMetadata>& metadata)
{
    JPEGImageFormat jpeg;
    PNGImageFormat png;
    
    is.setPosition (0);
    if (jpeg.canUnderstand (is))
    {
        is.setPosition (0);
        return loadJPEGMetadataFromStream (metadata, is);
    }
    
    is.setPosition (0);
    if (png.canUnderstand (is))
    {
        is.setPosition (0);
        return loadPNGMetadataFromStream (metadata, is);
    }
    
    return false;
}
