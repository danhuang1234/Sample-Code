// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
//TODO: Segfault for statistic input
//TODO: Fix median
#include "P2random.h"
#include <iostream>
#include <getopt.h>
#include <deque>
#include <queue>
using namespace std;

struct Zombie {
    public:
    string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    uint32_t active_rounds;
    uint32_t ETA;
    bool alive;
};

struct Options {
  bool v_output = false;
  bool m_output = false;
  bool s_output = false;
  uint32_t N = 0;

};  // Options{}
struct MedianLargeCompare{
  public:
  bool operator()(Zombie * a, Zombie * b){
    return a->active_rounds >= b->active_rounds;
  }
};
struct MedianSmallCompare{
  public:
  bool operator()(Zombie * a, Zombie * b){
    return a->active_rounds < b->active_rounds;
  }
};
struct ActiveCompare{
  public:
  bool operator()(Zombie * a, Zombie * b){
    if(a->active_rounds != b->active_rounds){
      return a->active_rounds < b->active_rounds;
    }
    else{
      return a->name > b->name;
    }
  }
};
struct OtherCompare{
  public:
  bool operator()(Zombie * a, Zombie * b){
    if(a->active_rounds != b->active_rounds){
      return a->active_rounds > b->active_rounds;
    }
    else{
      return a->name > b->name;
    }
  }
};
struct ZombieCompare{
  public:
  bool operator()(Zombie * a, Zombie * b){
    if(a->ETA != b->ETA){
      return a->ETA > b->ETA;
    }
    else if(a->health != b->health){
      return a->health > b->health;
    }
    else {
      return a->name > b->name;
    }
  }
};

class TowerDefense{
    public:
    //initializes everything
    TowerDefense(Options& option){
      string junk;
      getline(cin, junk);
      cin >> junk >> quiver_capacity >> junk >> seed
      >> junk >> max_distance >> junk >> max_speed >> junk >> max_health >> junk;
      gen.initialize(seed, max_distance, max_speed, max_health);
      verbose = option.v_output;
      median = option.m_output;
      N = option.N;
      stats = option.s_output;
      num_arrows = quiver_capacity;
      round = 0;
      end = false;
      alive = true;
      cin >> junk >> new_round;
      runningMedian = 0;
      Zombie * yeet = new Zombie;
      yeet->active_rounds = 10000;
      yeet->alive = false;
      yeet->name = "UNKNOWN";
      yeet->speed = 0;
      yeet->health = 0;
      yeet->ETA = 0;
      yeet->distance = 0;
      upper.push(yeet);
      masterList.push_back(yeet);
      Zombie * yote = new Zombie;
      yote->active_rounds = 0;
      yote->alive = false;
      yote->name = "UNKNOWN";
      yote->distance = 0;
      yote->ETA = 0;
      yote->health = 0;
      yote->speed= 0;
      lower.push(yote);
      masterList.push_back(yote);
      //seed the median PQs
    }

    void deleteZombies(){
      for(size_t i = 0; i < masterList.size(); i++){
        delete masterList[i];
      }
    }
    void play(){
       while(!end){
        round++;
        //Step 1
        if(verbose){
          cout << "Round: " << round << '\n';
        }
        //Step 2:refill quiver
        num_arrows = quiver_capacity;
        //Step 3: All active zombies advance toward you, updated in the order that they were created.
        updateZombies();
        //Step 4: Check dead
        if(!end){
          //Step 5
          if(new_round == round){
            makeZombies();
          }
          //Step 6
          shootZombies();
          //Step 7
          if(median && !killed.empty()){
            if(upper.size() == lower.size()){
              runningMedian = (upper.top()->active_rounds + lower.top()->active_rounds) / 2;
            }
            else if(upper.size() > lower.size()){
              runningMedian = upper.top()->active_rounds;
            }
            else{
              runningMedian = lower.top()->active_rounds;
            }

            cout << "At the end of round " << round << ", the median zombie lifetime is " << runningMedian << '\n';
            //find median
          }
          //check end
          if(shooter.empty() && new_round <= round){
            end = true;
          }
        }
      }
    }
    void shootZombies(){
      while(num_arrows != 0 && !shooter.empty()){
        if(num_arrows < shooter.top()->health){
          shooter.top()->health -= num_arrows; 
          num_arrows = 0;
        }
        else{
          num_arrows -= shooter.top()->health;
          shooter.top()->health = 0;
          //update median
          if(shooter.top()->active_rounds >= upper.top()->active_rounds){
            upper.push(shooter.top());
          }
          else{
            lower.push(shooter.top());
          }
          if(upper.size()-lower.size() == 2){
            lower.push(upper.top());
            upper.pop();
          }
          else if(lower.size() - upper.size() == 2){
            upper.push(lower.top());
            lower.pop();
          }

          killed.push_back(shooter.top());
          lastdeadzomb = shooter.top()->name;
          shooter.top()->alive = false;
          if(verbose){
            cout << "Destroyed: " << shooter.top()->name << " (distance: " << shooter.top()->distance 
            << ", speed: " << shooter.top()->speed << ", health: " << shooter.top()->health << ")\n";
          }
          shooter.pop();
        }
      }
    }

    void makeZombies(){
      //DEBUGGEEDDD POG
      //make zombie
      string junk;
      uint32_t num_rand;
      uint32_t num_named;
      cin >> junk >> num_rand >> junk >> num_named; 
      for(size_t i = 0; i < num_rand; i++){
        Zombie * hold = new Zombie;
        hold->name  = P2random::getNextZombieName();
        hold->distance = P2random::getNextZombieDistance();
        hold->speed    = P2random::getNextZombieSpeed();
        hold->health   = P2random::getNextZombieHealth();
        hold->ETA = hold->distance/hold->speed;
        hold->alive = true;
        hold->active_rounds = 1;

        shooter.push(hold);
        masterList.push_back(hold);
        if(verbose){
          cout << "Created: " << hold->name << " (distance: " << hold->distance 
          << ", speed: " << hold->speed << ", health: " << hold->health << ")\n";
        }
      }
      //make named
      for(size_t i = 0; i < num_named; i++){
        Zombie * named = new Zombie;
        cin >> named->name >> junk >> named->distance >> junk 
        >> named->speed >> junk >> named->health;
        named->ETA = named->distance/named->speed;
        named->alive = true;
        named->active_rounds = 1;
        shooter.push(named);
        masterList.push_back(named);
        if(verbose){
          cout << "Created: " << named->name << " (distance: " << named->distance 
          << ", speed: " << named->speed << ", health: " << named->health << ")\n";
        }
      }
      //set up cin
      cin >> junk >> junk >> new_round;
    }

    void updateZombies(){
      for(size_t i = 0; i < masterList.size(); i++){
        if(masterList[i]->alive){
          masterList[i]->distance -= min(masterList[i]->distance, masterList[i]->speed);
          masterList[i]->active_rounds++;
          masterList[i]->ETA--;
          if(verbose){
            cout << "Moved: " << masterList[i]->name << " (distance: " << masterList[i]->distance 
            << ", speed: " << masterList[i]->speed << ", health: " << masterList[i]->health << ")\n";
          }
          if(masterList[i]->distance == 0 && lastlivezomb.empty()){
            lastlivezomb = masterList[i]->name;
            alive = false;
            end = true;
          }
        }
      }
      return;
    }

    void checkPrio(){
      while(!shooter.empty()){
        cout << shooter.top()->name << " " << shooter.top()->ETA << " " << shooter.top()->health << endl;
        shooter.pop();
      }
    }
    void print(){
      cout << "Quiver capacity: " << quiver_capacity << "\n Seed: " << seed << endl << max_distance << endl << max_speed << endl << max_health << endl;
    }
    
    void printStats(){
      if(alive){
        cout << "VICTORY IN ROUND " << round << "! " << lastdeadzomb << " was the last zombie.\n";
      }
      else{
        cout << "DEFEAT IN ROUND " << round << "! " << lastlivezomb << " ate your brains!\n";
      }
      if(stats){
        //put active zombies into calculation for most active
        cout << "Zombies still active: " << shooter.size() << '\n';
        while(!shooter.empty()){
          if(shooter.top()->active_rounds >= upper.top()->active_rounds){
            upper.push(shooter.top());
          }
          else{
            lower.push(shooter.top());
          }
          shooter.pop();
          if(upper.size()-lower.size() == 2){
            lower.push(upper.top());
            upper.pop();
          }
          else if(lower.size() - upper.size() == 2){
            upper.push(lower.top());
            lower.pop();
          }
        }
        cout << "First zombies killed:\n";
        if(killed.size() < N){
          for(size_t i = 0; i < killed.size(); i++){
            cout << killed[i]->name << " " << i+1 << '\n';
          }
          cout << "Last zombies killed:\n";
          for(size_t i = killed.size(); i > 0; i--){
            cout << killed[i-1]->name << " " << i << '\n';
          }
        }
        else{
          for(size_t i = 0; i < N; i++){
            cout << killed[i]->name << " " << i+1 << '\n';
          }
          cout << "Last zombies killed:\n";
          for(size_t i = N; i > 0; i--){
            cout << killed[(killed.size() - (N+1)) + i]->name << " " << i << '\n';
          }
        }


        priority_queue<Zombie *, vector<Zombie *>, ActiveCompare> max(masterList.begin(), masterList.end());
        priority_queue<Zombie *, vector<Zombie *>, OtherCompare> min(masterList.begin(), masterList.end());
        max.pop();
        min.pop();
        if(max.size() < N+1){
          
          cout << "Most active zombies:\n";
          for(size_t i = max.size(); i > 1; i--){
          cout <<max.top()->name << " " << max.top()->active_rounds << '\n';
            max.pop();
          }
          cout << "Least active zombies:\n";

          for(size_t i = min.size(); i > 1; i--){
            cout << min.top()->name << " " << min.top()->active_rounds << '\n';
            min.pop();
          }

          return;
        }
        cout << "Most active zombies:\n";
        for(size_t i = 0; i< N; i++){
          cout <<max.top()->name << " " << max.top()->active_rounds << '\n';
            max.pop();
        }

        cout << "Least active zombies:\n";

        for(size_t i = 0; i< N; i++){
          cout << min.top()->name << " " << min.top()->active_rounds << '\n';
            min.pop();
        }


        
        /*
        if(upper.size() >= N + 1){
          while(upper.size() != N + 1){
            if(lower.size() < N + 1){
              lower.push(upper.top());
            }
            upper.pop();
          }
          vector<Zombie *> hold;
          for(size_t i = 0; i < N; i++){
            if(lower.size() < N + 1){
              lower.push(upper.top());
            }
            hold.push_back(upper.top());
            upper.pop();
          }
          for(size_t i = N; i > 0; i--){
            cout << hold.back()->name << " " << hold.back()->active_rounds << '\n';
            hold.pop_back();
          }

        }
        else if(lower.size() + upper.size() - 2 >= N){
          while(upper.size() < N + 1){
            upper.push(lower.top());
            lower.pop();
          }
          while(upper.size() != N + 1){
            lower.push(upper.top());
            upper.pop();
          }
          vector<Zombie *> hold;
          for(size_t i = 0; i < N; i++){
            hold.push_back(upper.top());
            lower.push(upper.top());
            upper.pop();
          }
          for(size_t i = N; i > 0; i--){
            cout << hold.back()->name << " " << hold.back()->active_rounds << '\n';
            hold.pop_back();
          }
          //DO THIS
          //write case for not enough in upper but enough in both
        }
        else{
          while(!lower.empty()){
            upper.push(lower.top());
            lower.pop();
          }
          //pop out the lowest (seed) value
          upper.pop();
          vector<Zombie *> hold;
          for(size_t i = upper.size(); i > 1; i--){
            hold.push_back(upper.top());
            upper.pop();
          }
          for(size_t i = hold.size(); i > 0; i--){
            cout << hold[i - 1]->name << " " << hold[i - 1]->active_rounds << '\n';
          }
          cout << "Least active zombies:\n";
          for(size_t i = 0; i < hold.size(); i++){
            cout << hold[i]->name << " " << hold[i]->active_rounds << '\n';
          }
          //write case for not enough in either to fulfill N
          //print both most and least
          return;
        }

        cout << "Least active zombies:\n";
        if(lower.size() >= N + 1){
          
          while(lower.size() != N+1){
            lower.pop();
          }
          vector<Zombie *> hold;
          for(size_t i = 0; i < N; i++){
            hold.push_back(lower.top());
            lower.pop();
          }
          for(size_t i = 0; i < N; i++){
            cout << hold[i]->name << " " << hold[i]->active_rounds << '\n';
          }
        }
        */
        
      }
    }

    private:
    uint32_t seed;
    uint32_t max_distance;
    uint32_t max_speed;
    uint32_t max_health;
    uint32_t quiver_capacity;
    bool verbose;
    bool median;
    bool stats;
    P2random gen;
    uint32_t N;
    uint32_t num_arrows;
    uint32_t round;
    bool end;
    bool alive;
    uint32_t runningMedian;
    uint32_t new_round;
    string lastlivezomb;
    string lastdeadzomb;

    vector<Zombie*>  masterList;
    priority_queue<Zombie*, vector<Zombie*> , ZombieCompare> shooter;

    vector<Zombie*> killed;

    //median
    priority_queue<Zombie *, vector<Zombie *>, MedianLargeCompare> upper;
    priority_queue<Zombie *, vector<Zombie *>, MedianSmallCompare> lower;
    
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
    {"verbose", no_argument, nullptr, 'v'},
    {"statistics", required_argument, nullptr, 's'},
    {"median", no_argument, nullptr, 'm'},
    {"help", no_argument, nullptr, 'h'},
    { nullptr, 0, nullptr, '\0' }
  };  // long_options[]

  // TODO: Fill in the double quotes, to match the mode and help options.
  while ((choice = getopt_long(argc, argv, "vs:mh", long_options, &index)) != -1) {
    switch (choice) {
      case 'h':
        printHelp();
        exit(0);

      case 's': {  // Need a block here to declare a variable inside a case
        string arg{optarg};
        
        options.s_output = true;
        options.N = stoi(arg);
        
        break;
      }  // case 'm'

      case 'm': {
        options.m_output = true;

        break;
      }
      case 'v': {
        options.v_output = true;
        break;
      }
      default:
        cerr << "Error: invalid option" << endl;
        exit(1);
    }  // switch ..choice
  }  // while

}  // getMode()



int main(int argc, char * argv[]){
    // This should be in all of your projects, speeds up I/O
    ios_base::sync_with_stdio(false);

    Options start;
    getMode(argc, argv, start);
    TowerDefense me(start);
    me.play();
    me.printStats();
    me.deleteZombies();
    // me.checkPrio();
}