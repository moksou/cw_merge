#include "globaldefs.h"
#include "wad.h"

int main(int argc, char* argv[])
{
    wadfile_t* wad[argc - 1];

/*    while ((c = getopt(argc, Vargv, "ho:")) != -1) {
        switch (c) {
            case 'h':
                fprintf(stderr, "Usage: %s filename1.wad filename2.wad -o result.wad\n");
                return 1;
            case 'o':
                destname = optarg;
                break;
*/            


    if (argc < 2) {
        fprintf(stderr, "Error: too few arguments\n");
        return 1;
    }
    for (int i = 0; i < argc - 1; i++) {
        wad[i] = wad_load(argv[i + 1]);
    }


    for (int i = 0; i < argc - 1; i++) {
        wad_cleanup(wad[i]);
    }
}
