#include <iostream>

#define BLOCKLISTPATH   "C:\\Windows\\System32\\drivers\\etc\\hosts";

int main(int argc, char** argv){
    if(argc == 2){
        if(argv[1] == "\\h"){
            std::cout << "commands:\n";
            std::cout << "add <urls[]>    - <url>= String[] of the urls you want to block";
            std::cout << "del <urls[]>    - <url>= String[] of the urls you want to delete from block list";
            std::cout << "list            - List all blocked urls";
        }
        else if(argv[1] == "list"){
            showBlockList();
        }
    }

    if(argc < 3){
        showUsage();
    }
    
}

void showUsage(){
    std::cout << "usage: <command> <urls> or type \\h for help";
}

void showBlockList(){
    std::cout << "showing list";
}

bool isUrl(std::string str){

}

void addUrl(std::string url){

}

void deleteUrl(std::string url){

}