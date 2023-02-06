#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>


struct alar_header{
    char name[100];
    char mode[8];
    char owner[8];
    char group[8];
    char size[12];
    char modified[12];
    char checksum[8];
    char type[1];
    char link[100];
    char padding[255];
};
void alar_unpack(const char *alar_path){
    int alar_file,ret;
    char buf[512];
    alar_file = open(alar_path, O_RDONLY);
        if (alar_file == -1) {
            perror("Error opening alar file");
            return;
        }

        //перебор блоков в архиве
        while ((ret = read(alar_file, buf, 512)) == 512) {
            // получение файла заголовка для след. файла
            struct alar_header * header = (struct alar_header *)buf;

            // проверка конца архива
            if (strcmp(header->name, "") == 0)
                break;

            // создание файла
            int file = open(header->name, O_WRONLY | O_CREAT | O_TRUNC,
                            (mode_t)strtol(header->mode, (char **)NULL, 8));

            if (file == -1) {
                perror("Error creating file");
                return;
            }

            // получение размера файла
            int size = (int)strtol(header->size, (char **)NULL, 8);

            // запись информации в файл
            int bytes_left = size;
            while (bytes_left > 0) {
                ret = read(alar_file, buf, 512);
                write(file, buf, ret);
                bytes_left -= ret;
            }

            close(file);
        }
        close(alar_file);

}
void fexpand(int f, size_t amount, int value){
    while( amount-- ){
        write(f, &value, 1);
    }
}

void alar_add(int alar_file, const char *dir_path){
    DIR *dir;
    struct dirent *entry;
    //проверка дирректории
    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir");
        return;
    }

    //обход файлов в дирректории и запись в архив
    while ((entry = readdir(dir)) != NULL) {
        struct stat stat_buf;
        char path[1024];

        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        if (stat(path, &stat_buf) == -1) {
            perror("stat");
            continue;
        }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
             continue;
        }

        //Получаем текущую позицию; округляем до кратного 512
        ssize_t read_bytes;
        long index = lseek(alar_file, 0,SEEK_CUR  );
        size_t offset = index % 512;
        if( offset != 0 ){
            fexpand( alar_file, 512 - offset, 0);
        }
        //сохранение индекса заголовка
        index = lseek(alar_file, 0,SEEK_CUR  );
        fexpand( alar_file, sizeof(struct alar_header), 0 );
        //открытия файла для чтения
        int input = open(path, O_RDONLY);
        if( input < 0 ){
            perror("Failed to open for reading\n");
            return;
        }
        //копирование содержания файла в архив
        char buffer[513];
        while ((read_bytes = read (input, buffer, 512)) > 0)
        {
            write (alar_file, buffer, read_bytes);
        }
        //конец, чтобы рассчитать размер файла
        long end = lseek(alar_file, 0,SEEK_CUR  );
        //округлить размер файла до значения, кратного 512 байтам
        offset = end % 512;
        if( end != 0 ){
            fexpand( alar_file, 512 - offset, 0);
        }
        //заполнить заголовок архива
        struct alar_header header;
        memset( &header, 0, sizeof( struct alar_header ) );
        snprintf( header.name, 100, "%s",entry->d_name);
        snprintf(header.mode, 8, "%06o ", stat_buf.st_mode);
        snprintf(header.owner, 8, "%06o ", stat_buf.st_uid);
        snprintf(header.group, 8, "%06o ", stat_buf.st_gid);
        snprintf( header.size, 12, "%011zu ", end - 512 - index );
        snprintf( header.modified, 12, "%011zu ", stat_buf.st_mtime );
        memset( header.checksum, ' ', 8);
        header.type[0] = '0';

        //вычислить контрольную сумму
        size_t checksum = 0;
        int i;
        const unsigned char* bytes = (const unsigned char *)( &header );
        for( i = 0; i < sizeof( struct alar_header ); ++i ){
            checksum += bytes[i];
        }

        snprintf( header.checksum, 8, "%06zu", checksum );

        //Сохраните новый конец, чтобы вернуться к нему после написания заголовка
        end = lseek(alar_file, 0,SEEK_CUR  );

        //Записать заголовок
        lseek( alar_file, index, SEEK_SET );
        write(alar_file, bytes, sizeof( struct alar_header ));


        //вернуться в конец
        lseek( alar_file, end, SEEK_SET );
        close( input );

    }
}

int main( int argc, char* argv[] ){
    if( argc > 1 ){
        if (strstr(argv[1],".alar")){
            alar_unpack(argv[1]);
            return 0;
        }
        int alar = open("archive.alar",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
        if( !alar ){
            perror("Failed to open  or writing");
            return 1;
        }

       alar_add( alar, argv[1] );

        //Pad out the end of the alar file
        fexpand( alar, 1024, 0);
        close( alar );
        return 0;
    }
    perror("Please specify some file names!\n");
    return 0;
}
