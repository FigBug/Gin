/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

class SharedMemory::Impl
{
public:
    Impl (String name, int sz) : size (sz)
    {
        bool needsInit = false;
        
        shareName = "/jshm" + File::createLegalFileName (name);
        
        fd = shm_open (shareName.toRawUTF8(),  O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (fd == -1)
            fd = shm_open (shareName.toRawUTF8(),  O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        else
            needsInit = true;
        
        if (fd == -1)
        {
            size = 0;
            return;
        }
        
        if (needsInit && ftruncate (fd, size) == -1)
        {
            size = 0;
            return;
        }
        
        struct stat statbuf;
        if (fstat (fd, &statbuf) == -1)
        {
            size = 0;
            return;
        }
        
        size = int (statbuf.st_size);
        
        data = mmap (0, size_t (size), PROT_WRITE, MAP_SHARED, fd, 0);
        jassert (data != nullptr);
        
        if (data != nullptr && needsInit)
            memset (data, 0, sizeof (data));
    }
    
    ~Impl()
    {
        if (data != nullptr)
            munmap (data, size_t (size));
        
        if (fd != -1)
            close (fd);
        
        shm_unlink (shareName.toRawUTF8());
    }
    
    static void remove (const String& name)
    {
        String shareName = "/jshm" + File::createLegalFileName (name);
        shm_unlink (shareName.toRawUTF8());
    }
    
    String shareName;
    int size;
    void* data = nullptr;
    int fd;
};

SharedMemory::SharedMemory (const String& name, int size)
{
    impl = new Impl (name, size);
}

SharedMemory::~SharedMemory()
{
}

void* SharedMemory::getData()
{
    return impl->data;
}

int SharedMemory::getSize()
{
    return impl->size;
}

void SharedMemory::remove (const String& name)
{
    Impl::remove (name);
}

