// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725
#include <algorithm>
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <getopt.h>
using namespace std;

void printHelp();

enum class Mode {
  kNone = 0,
  kQueue,
  kStack,
};  // Mode{}

struct Options {
  Mode mode = Mode::kNone;
  bool map_bool = true;
};  // Options{}

struct Data{
  char point;
  bool discovered;
};

struct Location {
  uint32_t row;
  uint32_t col;
  uint32_t color;
};
//reading map from cli




class Algorithm{
    public:
    
    Algorithm(Options option){
      output_map = option.map_bool;
      if(option.mode == Mode::kQueue){
        stack = false;
      }
      else{
        stack = true;
      }
      string junk;
      cin >> colors >> rows >> cols;
      Data blank;
      blank.point = ' ';
      blank.discovered = false;
      map.resize(rows, vector<Data>(cols, blank));
      answer.resize(colors + 1, vector<vector<char>>(rows, vector<char>(cols, '#')));
      char hold;
      cin >> hold;
      while (hold == '/'){
        getline(cin, junk);
        cin >> hold;
      }
      
      for(uint32_t i = 0; i < rows; i++){
        for(uint32_t j = 0; j < cols; j++){
          if(i == 0 && j == 0){
            map[i][j].point = hold;
          }
          else{
            cin >> hold;
            map[i][j].point = hold;
          }
        }
      }
    }

    bool errorchecking(){
      int num_start = 0;
      int num_end = 0;
      if(colors > 26){
        cout << "Error: Invalid numColor \n";
        return true;
      }
      if(rows < 1){
        cout << "Error: Invalid height \n";
        return true;
      }
      if(cols < 1){
        cout << "Error: Invalid width \n";
        return true;
      }

      for(uint32_t i = 0; i < rows; i++){
        for(uint32_t j = 0; j < cols; j++){
          //valid start
          if(map[i][j].point == '@'){
              num_start++;            
          }
          //valid end
          if(map[i][j].point == '?'){
              num_end++;
          }
          
          //check for invalid character
          if(map[i][j].point != '.' && map[i][j].point != '#' && map[i][j].point != '?'&& map[i][j].point != '@' 
          && map[i][j].point != '^' 
          && !(int(map[i][j].point) >= 65 && int(map[i][j].point) < 65 + static_cast<int>(colors)) 
          && !(int(map[i][j].point) >= 97 && int(map[i][j].point) < 97 + static_cast<int>(colors))){
            cout << map[i][j].point << endl;
              cout << "Error: invalid character in map \n";
              exit(1);
          }
          
        }
      }

      if(num_end > 1 || num_start > 1){
        cout << "Error: Too many starts or ends \n";
        return true;
      }
      if(num_end == 0 || num_start == 0){
        cout << "Error: Not enough starts or ends \n";
        return true;
      }

      return false;
    }
    void Findstart(){
      for(uint32_t i = 0; i < rows; i++){
        for(uint32_t j = 0; j < cols; j++){
          //valid start
          if(map[i][j].point == '@'){
              start.color = 0;
              start.row = i;
              start.col = j;
              answer[start.color][start.row][start.col] = map[i][j].point;
              search_container.push_back(start);
              map[i][j].discovered = true;
              
          }
        }
      }
    }

    bool solve(){
      while (!(search_container.empty())){
        if(stack){
          search = search_container.back();
          search_container.pop_back();
        }
        else{
          search = search_container.front();
          search_container.pop_front();
        }
        if(isButton()){
          Location hold = search;
          if(map[search.row][search.col].point == '^' ){
            hold.color = 0;
            if(answer[hold.color][hold.row][hold.col] == '#'){
              search_container.push_back(hold);
              answer[hold.color][hold.row][hold.col] = char(search.color + 96);
            }
          }
          else{
            hold.color = int(map[search.row][search.col].point) - 96;
            if(answer[hold.color][hold.row][hold.col] == '#'){
              search_container.push_back(hold);
              answer[hold.color][hold.row][hold.col] = char(search.color + 96);
            }
          }
        }
        else{
          //step 6
          if(search.row > 0){
            search.row -= 1;
            if(step6()){
              answer[search.color][search.row][search.col] = 'S';
              search_container.push_back(search);
              map[search.row][search.col].discovered = true;
              if(isFlag()){
                return true;
              }
            }
            search.row += 1;
          }
          if(search.col < cols - 1){
            search.col += 1;
            if(step6()){
              answer[search.color][search.row][search.col] = 'W';
              search_container.push_back(search);
              map[search.row][search.col].discovered = true;
              if(isFlag()){
                return true;
              }
            }
            search.col -= 1;
          }
          if(search.row < rows - 1){
            search.row += 1;
            if(step6()){
              answer[search.color][search.row][search.col] = 'N';
              search_container.push_back(search);
              map[search.row][search.col].discovered = true;
              if(isFlag()){
                return true;
              }
            }
            search.row--;
          }
          if(search.col > 0){
            search.col--;
            if(step6()){
              answer[search.color][search.row][search.col] = 'E';
              map[search.row][search.col].discovered = true;
              search_container.push_back(search);
              if(isFlag()){
                return true;
              }
            }
            search.col++;
          }    
        }
      }
      NoSolution();
      return false;
    }
    void NoSolution(){
      cout << "No solution. \nDiscovered: \n";
      for(uint32_t i = 0; i < rows; i++){
        for(uint32_t j = 0; j < cols; j++){
          if(map[i][j].discovered){
            cout << map[i][j].point;
          }
          else{
            cout << "#";
          }
        }
        cout << "\n";
      }
      end = true;
    }
    bool isFlag(){
      if(map[search.row][search.col].point == '?'){
        flag = search;
        return true;
      }
      else {
        return false;
      }
    }

    void backtrace(){
      if(output_map){
        //map backtrace
        //TODO: FIX BUGS, missing percent sign
        while (flag.row != start.row || flag.col != start.col || flag.color != start.color){
          if(answer[flag.color][flag.row][flag.col] == 'N'){
            map_helper();
            flag.row--;
          }
          else if(answer[flag.color][flag.row][flag.col] == 'S'){
            map_helper();
            flag.row++;
          }
          else if(answer[flag.color][flag.row][flag.col] == 'E'){
            map_helper();
            flag.col++;
          }
          else if(answer[flag.color][flag.row][flag.col] == 'W'){
            map_helper();
            flag.col--;
          }
          else{
            Location hold = flag;
            flag.color = int(answer[flag.color][flag.row][flag.col]) - 96;
            answer[hold.color][hold.row][hold.col] = '@';
            if(answer[flag.color][flag.row][flag.col] == 'N'){
              answer[flag.color][flag.row][flag.col] = '%';
              flag.row--;
            }
            else if(answer[flag.color][flag.row][flag.col] == 'S'){
              answer[flag.color][flag.row][flag.col] = '%';
              flag.row++;
            }
            else if(answer[flag.color][flag.row][flag.col] == 'E'){
              answer[flag.color][flag.row][flag.col] = '%';
              flag.col++;
            }
            else if(answer[flag.color][flag.row][flag.col] == 'W'){
              answer[flag.color][flag.row][flag.col] = '%';
              flag.col--;
            }
          }
        }


        for(uint32_t c = 0; c <= colors; c++){
          for(uint32_t i = 0; i < rows; i++){
            for(uint32_t j = 0; j < cols; j++){
              if(answer[c][i][j] != '@' && answer[c][i][j] != '+' && answer[c][i][j] != '%'){
                answer[c][i][j] = map[i][j].point;
              }
              if((map[i][j].point == char(64 + c) || map[i][j].point == char(96+c)|| (c == 0 && map[i][j].point == '^')) && answer[c][i][j] != '@' && answer[c][i][j] != '+' && answer[c][i][j] != '%'){
                answer[c][i][j] = '.';
              }
              
            }
          }
        }
        for(uint32_t c = 1; c<=colors; c++){
          if(answer[c][start.row][start.col] == '@'){
            answer[c][start.row][start.col] = '.';
          }
        }

        print_answer_map();
      }
      else{
        search_container.clear();
        while(answer[flag.color][flag.row][flag.col] != '@'){
          search_container.push_back(flag);
          if(answer[flag.color][flag.row][flag.col] == 'N'){
            flag.row--;
          }
          else if(answer[flag.color][flag.row][flag.col] == 'S'){
            flag.row++;
          }
          else if(answer[flag.color][flag.row][flag.col] == 'E'){
            flag.col++;
          }
          else if(answer[flag.color][flag.row][flag.col] == 'W'){
            flag.col--;
          }
          else{
            flag.color = int(answer[flag.color][flag.row][flag.col]) - 96;
          }
        }
        search_container.push_back(flag);
        //list backtrace

        //list print
        while(!search_container.empty()){
          if(search_container.back().color == 0){
            cout << "(^, (" << search_container.back().row << ", " << search_container.back().col << "))\n";
          }
          else{
            cout << '(' << char(search_container.back().color + 96) << ", (" << search_container.back().row << ", " << search_container.back().col << "))\n";
          }
          search_container.pop_back();
        }
      }
    }

    void map_helper(){
      if(map[flag.row][flag.col].point != '?' && answer[flag.color][flag.row][flag.col] != '%' ){
        answer[flag.color][flag.row][flag.col] = '+';
      }
    }
    
    void print_answer_map(){
      for(uint32_t c = 0; c <= colors; c++){
        if(c != 0){
          cout << "// color " << char(96 + c) << "\n";
        }
        else{
          cout << "// color ^\n";
        }
        for(uint32_t i = 0; i < rows; i++){
          for(uint32_t j = 0; j < cols; j++){
            cout << answer[c][i][j];
          }
          cout << "\n";
        }
      }
    }
    bool step6(){
      if(!isDiscovered() && (map[search.row][search.col].point != '#')){
        if(int(map[search.row][search.col].point) > 64 
        && int(map[search.row][search.col].point) < 91
        && !isOpenDoor()){
          return false;
        }
        return true;
      }
      else {return false;}
    }
    bool isOpenDoor(){
      if(int(map[search.row][search.col].point) == 65 + static_cast<int>(search.color) -1){
        return true;
      }
      else {return false; }
    }
    void print(){
        for(uint32_t i = 0; i < map.size(); i++){
            for(uint32_t j=0; j < map[0].size(); j++){
                cout << map[i][j].point;
            }
            cout << '\n';
        }
    }


    bool isDiscovered(){
      if(answer[search.color][search.row][search.col] == '#'){
        return false;
      }
      else{
        return true;
      }
    }
    bool isButton(){
      if(int(map[search.row][search.col].point) >= 94 && int(map[search.row][search.col].point) <= 96 + static_cast<int>(colors)){
        if(search.color == 0){
          if(map[search.row][search.col].point != '^'){
            return true;
          }
          return false;
        }
        else if (int(map[search.row][search.col].point) != 96 + static_cast<int>(search.color)){
          return true;
        }
        else{
          return false;
        }
      }
      else{
        return false;
      }
    }
    

    private:
    uint32_t colors;
    uint32_t rows;
    uint32_t cols;
    std::vector<std::vector<Data>> map;
    //deque to hold search container
    Location start;
    Location search;
    Location flag;

    //backtracing (hold)
    deque<Location> search_container;

    //backtracing (answer)
    std::vector<std::vector<std::vector<char>>> answer;

    //getopt for type
    bool output_map; //true for map, false for list
    bool stack; //true for stack, false for queue
    bool end;
    

};

// Print help for the user when requested.
// argv[0] is the name of the currently executing program
void printHelp() {
  cout << "Your mom"<< endl;
}  // printHelp()


// TODO: Finish this function, look for individual TODO comments internally.
// Process the command line; there is no return value, but the Options
// struct is passed by reference and is modified by this function to send
// information back to the calling function.
void getMode(int argc, char * argv[], Options &options) {
  // These are used with getopt_long()
  opterr = false; // Let us handle all error output for command line options
  int choice;
  int index = 0;
  option long_options[] = {
    // TODO: Fill in two lines, for the "mode" ('m') and
    // the "help" ('h') options.
    {"output", required_argument, nullptr, 'o'},
    {"stack", no_argument, nullptr, 's'},
    {"queue", no_argument, nullptr, 'q'},
    {"help", no_argument, nullptr, 'h'},
    { nullptr, 0, nullptr, '\0' }
  };  // long_options[]

  // TODO: Fill in the double quotes, to match the mode and help options.
  bool stack_or_queue = false;
  while ((choice = getopt_long(argc, argv, "ho:sq", long_options, &index)) != -1) {
    switch (choice) {
      case 'h':
        printHelp();
        exit(0);

      case 's': {  // Need a block here to declare a variable inside a case
        if(stack_or_queue){
            cout << "Error: Can not have both stack and queue \n";
            exit(1);
        }
        stack_or_queue = true;
        
        options.mode = Mode::kStack;
        
        break;
      }  // case 'm'

      case 'q': {
        if(stack_or_queue){
            cout << "Error: Can not have both stack and queue \n";
            exit(1);
        }
        stack_or_queue = true;

        options.mode = Mode::kQueue;
        break;
      }
      case 'o': {
        string arg{optarg};
        if(arg != "map" && arg != "list"){
            cout << "Error: Invalid output mode \n";
            //unfinished
            exit(1);
        }
        else if(arg == "list"){
            options.map_bool = false;
        }
        break;
      }
      default:
        cerr << "Error: invalid option" << endl;
        exit(1);
    }  // switch ..choice
  }  // while

  if (options.mode == Mode::kNone) {
    cerr << "Error: no mode specified" << endl;
    exit(1);
  }  // if ..mode
}  // getMode()

int main(int argc, char *argv[]) {
    // This should be in all of your projects, speeds up I/O
    ios_base::sync_with_stdio(false);

    Options option;
    getMode(argc, argv, option);
    Algorithm puzzles(option);

    if(puzzles.errorchecking()){
      exit(1);
    }

    puzzles.Findstart();
    if(!puzzles.solve()){
      return 0;
    }
    
    puzzles.backtrace();
    return 0;

}
