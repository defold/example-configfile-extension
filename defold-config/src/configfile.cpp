#include <stdint.h>
#include <stdlib.h>

#include <dmsdk/dlib/array.h>
#include <dmsdk/dlib/configfile.h>
#include <dmsdk/extension/extension.h>

#include "rot13.h"

#if 1
#define DEBUG printf
#else
#define DEBUG (void)
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// If we wish to return strings, we need to store them during the lifespan of the config file
struct SConfigStrings
{
    dmConfigFile::HConfig   m_Config;   // The config file the strings belong to
    dmArray<const char*>    m_Strings;  // the dynamically allocated strings
};

static dmArray<SConfigStrings*> g_ConfigStrings;

static SConfigStrings* FindConfigStringContainer(dmConfigFile::HConfig config, uint32_t* index)
{
    for (uint32_t i = 0; i < g_ConfigStrings.Size(); ++i)
    {
        SConfigStrings* container = g_ConfigStrings[i];
        if (container->m_Config == config)
        {
            if (index)
                *index = i;
            return container;
        }
    }
    return 0;
}

static void AllocateConfigStrings(dmConfigFile::HConfig config)
{
    if (g_ConfigStrings.Full())
        g_ConfigStrings.OffsetCapacity(1);

    SConfigStrings* container = new SConfigStrings;
    container->m_Config = config;
    g_ConfigStrings.Push(container);

    DEBUG("Allocated string container for config %p\n", config);
}

static void FreeConfigStrings(dmConfigFile::HConfig config)
{
    uint32_t index;
    SConfigStrings* container = FindConfigStringContainer(config, &index);
    if (container)
    {
        g_ConfigStrings.EraseSwap(index);

        uint32_t count = container->m_Strings.Size();
        for (uint32_t i = 0; i < count; ++i)
        {
            free((void*)container->m_Strings[i]);
        }
        delete container;

        DEBUG("Deleted %u strings\n", count);
    }
}

static void PutString(dmConfigFile::HConfig config, const char* string)
{
    SConfigStrings* container = FindConfigStringContainer(config, 0);
    if (container->m_Strings.Full())
        container->m_Strings.OffsetCapacity(8);
    container->m_Strings.Push(string);

    DEBUG("Stored string '%s' for container %p\n", string, config);
}

//////////////////////////////////////////////////////////////////////////////////////////////

static bool Matches(const char* key)
{
    // Each extension needs to decide what properties to override
    if (strstr(key, "secret.") == key)
        return true;
    return false;
}

static void ConfigFile_Create(dmConfigFile::HConfig config)
{
    AllocateConfigStrings(config);
}

static void ConfigFile_Destroy(dmConfigFile::HConfig config)
{
    FreeConfigStrings(config);
}

// The "default_value" is the actual value from the config file (or the default if it was missing)
// Here you are given an opportunity to override this value
static bool ConfigFile_GetString(dmConfigFile::HConfig config, const char* key, const char* default_value, const char** out)
{
    if (!Matches(key))
        return false;
    // the keys now start with "secret"

    char* s = strdup(default_value);
    rot13(s);

    DEBUG("Decrypted '%s'='%s' to '%s'\n", key, default_value, s);

    // Since the string may be stored for the duration of the config file, we need to store it
    PutString(config, s);

    *out = s;
    return true;
}
static bool ConfigFile_GetInt(dmConfigFile::HConfig config, const char* key, int32_t default_value, int32_t* out)
{
    if (!Matches(key))
        return false;
    // the keys now start with "secret"

    *out = default_value * 2; // Do some custom transformation
    return true;
}

static bool ConfigFile_GetFloat(dmConfigFile::HConfig config, const char* key, float default_value, float* out)
{
    if (!Matches(key))
        return false;
    // the keys now start with "secret"

    *out = default_value * 4; // Do some custom transformation
    return true;
}

DM_DECLARE_CONFIGFILE_EXTENSION(ConfigfileExtension, "ConfigfileExtension", ConfigFile_Create, ConfigFile_Destroy, ConfigFile_GetString, ConfigFile_GetInt, ConfigFile_GetFloat);



static dmExtension::Result ConfigFileExt_AppInitialize(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result ConfigFileExt_Initialize(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result ConfigFileExt_AppFinalize(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result ConfigFileExt_Finalize(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(ConfigFileExt, "ConfigFileExt", ConfigFileExt_AppInitialize, ConfigFileExt_AppFinalize, ConfigFileExt_Initialize, 0, 0, ConfigFileExt_Finalize)




//
