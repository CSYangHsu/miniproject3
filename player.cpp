#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <string>
#include <sstream>

using namespace std;

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(int  x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}

};

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
//private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard(array<array<int, SIZE>, SIZE> b){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = b[i][j];
            }
        }
    }
    OthelloBoard(const OthelloBoard& A){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = A.board[i][j];
            }
        }
        cur_player = A.cur_player;
        for(int i=0;i<3;i++){
            disc_count[i] = A.disc_count[i];
        }
    }
    OthelloBoard() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
//vector<Point> imppoint={(0,0),(0,7),(7,0),(7,7)};

/*
const int valuetable[8][8]={ 
     6, -3,  2,  2,  2,  2, -3,  6,
    -3, -4, -1, -1, -1, -1, -4, -3,
     2, -1,  1,  0,  0,  1, -1,  2,
     2, -1,  0,  1,  1,  0, -1,  2,
     2, -1,  0,  1,  1,  0, -1,  2,
     2, -1,  1,  0,  0,  1, -1,  2,
    -3, -4, -1, -1, -1, -1, -4, -3,
     6, -3,  2,  2,  2,  2, -3,  6
    };
*/


const int valuetable[8][8]={ 6, -3,  3,  2,  2,  3, -3,  6,
                            -3, -3, -1, -1, -1, -1, -3, -3,
                             3, -1,  1,  0,  0,  1, -1,  3,
                             2, -1,  0,  1,  1,  0, -1,  2,
                             2, -1,  0,  1,  1,  0, -1,  2,
                             3, -1,  1,  0,  0,  1, -1,  3,
                            -3, -3, -1, -1, -1, -1, -3, -3,
                             6, -3,  3,  2,  2,  3, -3,  6
    };

int get_value(OthelloBoard B)
{
    
   
    int validvalue=0;
    int enemy = 3 - player;
    int myvalue = 0, enemyvalue = 0;
    OthelloBoard EnemyBoard(B);
    B.cur_player = player;
    EnemyBoard.cur_player = enemy;
    
    for( int i = 0; i < 8; i++){
        for( int j = 0; j < 8; j++){
            Point p(i, j);
            if( B.board[i][j]==player ){
             
                myvalue += valuetable[i][j];
            }
            else if( B.board[i][j]==enemy ){
                enemyvalue +=valuetable[i][j];
            }
            
            /*if(B.is_spot_valid(p)){
                //我
                validvalue +=valuetable[i][j];
                //myvalue += 6;
            }*/
         
                
        }
    }
    
   //myvalue+=validvalue;
   
    return int( (myvalue - enemyvalue));//+ dif;  
 
}


int minimax(OthelloBoard b,int depth,int alpha,int beta,int cur_player)
{

    int enemy = 3 - player;
    if (depth==0||b.next_valid_spots.empty()) return get_value(b);
    if (cur_player==player)
    {
        int ans=-INT_MAX;
        for (int i=0;i<(int)b.next_valid_spots.size();i++)
        {
            OthelloBoard child(b);
            child.cur_player = cur_player;
            child.put_disc(b.next_valid_spots[i]);
            ans=max(ans,minimax(child,depth-1,alpha,beta,3 - child.cur_player));
            alpha=max(alpha,ans);
            if (alpha>=beta) break;
        }
        return ans;
    }
    else if( cur_player == enemy )
    {
        int ans=INT_MAX;
        for (int i=0;i<(int)b.next_valid_spots.size();i++)
        {
            OthelloBoard child(b);
            child.cur_player = cur_player;
            child.put_disc(b.next_valid_spots[i]);
            ans=min(ans,minimax(child,depth-1,alpha,beta,3 - child.cur_player));
            beta=min(beta,ans);
            if (beta<=alpha) break;
        }
        return ans;
    }

    return 0;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}
vector<Point> special;
Point a(0,0),b(0,7),c(7,0),d(7,7);
Point n1(1,1),n2(1,6),n3(6,1),n4(6,6);
Point m1(0,1),m2(1,0),m3(0,6),m4(6,0),m5(1,7),m6(7,1),m7(7,6),m8(6,7);

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = -1;
    int alpha=-INT_MAX;
    int beta=INT_MAX;
    for (int i=0;i<n_valid_spots;i++)
    {
        OthelloBoard nxt(board);
        nxt.cur_player=player;
        nxt.put_disc(next_valid_spots[i]);
        int cur=minimax(nxt,6,alpha,beta,3-player);

        if (cur>alpha){
            alpha=cur,index=i;
            Point p = next_valid_spots[i];
             // Remember to flush the output to ensure the last action is written to file.
            fout << p.x << " " << p.y << std::endl;

        } 
        
    }
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
