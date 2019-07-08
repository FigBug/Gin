/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if JUCE_WINDOWS
typedef enum _SECTION_INFORMATION_CLASS
{
    SectionBasicInformation,
    SectionImageInformation
} SECTION_INFORMATION_CLASS;

typedef struct _SECTION_BASIC_INFORMATION {
    PVOID         base;
    ULONG         attributes;
    LARGE_INTEGER size;
} SECTION_BASIC_INFORMATION;

typedef DWORD (WINAPI* NTQUERYSECTION) (HANDLE, SECTION_INFORMATION_CLASS, PVOID, ULONG, PULONG);

class SharedMemory::Impl
{
public:
    Impl (String name, int sz) : size (sz)
    {
        String shareName = "Local\\" + File::createLegalFileName (name);

        fileMapping = OpenFileMappingW (FILE_MAP_ALL_ACCESS, FALSE, shareName.toWideCharPointer());
        if (fileMapping == nullptr)
            fileMapping = CreateFileMappingW (INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sz, shareName.toWideCharPointer());

        if (HMODULE dll = LoadLibrary ("ntdll.dll"))
        {
            NTQUERYSECTION ntQuerySection = (NTQUERYSECTION) GetProcAddress (dll, "NtQuerySection");
            if (ntQuerySection != nullptr)
            {
                SECTION_BASIC_INFORMATION SectionInfo = { 0 };
                ntQuerySection (fileMapping, SectionBasicInformation, &SectionInfo, sizeof (SectionInfo), 0);

                if (SectionInfo.size.QuadPart > 0)
                    size = int (SectionInfo.size.QuadPart);
            }
            FreeLibrary (dll);
        }

        data = MapViewOfFile (fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, size);
    }

    ~Impl()
    {
        if (data != nullptr)
            UnmapViewOfFile (data);

        if (fileMapping != nullptr)
            CloseHandle (fileMapping);
    }

    static void remove (const String& name)
    {
        ignoreUnused (name);
    }

    HANDLE fileMapping = nullptr;

    int size = 0;
    void* data = nullptr;
};
#else
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

        data = mmap (nullptr, size_t (size), PROT_WRITE, MAP_SHARED, fd, 0);
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
#endif

SharedMemory::SharedMemory (const String& name, int size)
{
    impl = std::make_unique<Impl> (name, size);
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
