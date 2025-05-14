#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *buffer;

    if (argc < 2) return 1;

    if (memcmp(argv[1], "--dynamic", 5) == 0)
    {
        buffer = "{\n  \"type\": \"dynamic\",\n  \"description\": \"Dynamically configured behavior activated\"\n}";
        printf("%s\n", buffer);
    }
    else if (memcmp(argv[1], "--static", 5) == 0)
    {
        buffer = "{\n  \"type\": \"static\",\n  \"description\": \"Statically configured behavior activated\"\n}";
        printf("%s\n", buffer);
    }
    else if (memcmp(argv[1], "--help", 5) == 0)
    {
        buffer = "{\n  \"type\": \"help\",\n  \"description\": \"Available options: --dynamic, --static, --help\"\n}";
        printf("%s\n", buffer);
    }
    else if (memcmp(argv[1], "--all", 4) == 0)
    {
        buffer = "{\n  \"type\": \"all\",\n  \"description\": \"All options activated\"\n}";
        printf("%s\n", buffer);
    }
    else
    {
        buffer = "{\n  \"type\": \"unknown\",\n  \"description\": \"Unrecognized option\"\n}";
        printf("%s\n", buffer);
    }

    return 0;
}
 