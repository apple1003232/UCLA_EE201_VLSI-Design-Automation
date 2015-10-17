    #define MAX 0xffff
#define WINDOWSIZE 1000
#include <vector>
#include <set>

using namespace std;

class Point;
class Obstacle;

typedef vector<Point*>          Path;
typedef set<Point*>             PointSet;
typedef vector<Obstacle*>       ObstacleVector;
typedef vector<Path*>           PathVector;

//bool def_Small(Path* left, Path* right);



class Point {
    public:
        Point(int a, int b):
            x(a),
            y(b) {}

        Point(Point* p):
            x(p->x),
            y(p->y) {}
        
        Point()
            {
                Point(0,0);
            }

    public:
        int x;
        int y;
        PathVector* paths;
};

class Obstacle {
    public:
        Obstacle(int llx, int lly, int urx, int ury)
            {
                ll = new Point(llx, lly);
                ur = new Point(urx, ury);
            }

        Obstacle(Point* a, Point* b):
            ll(a),
            ur(b) {
            if(a->x > b->x) {
                ll = new Point(b);
                ur = new Point(a);
            }
            else {
                ll = new Point(a);
                ur = new Point(b);
            } 
        }

        ~Obstacle() {//Îö¹¹ 
            if (ll) delete ll;
            if (ur) delete ur;
        }

    public:
        Point* ll;      //the lower left corner
        Point* ur;      //the upper right corner
};

struct LessPath {
    bool operator ()(const Path* left, const Path* right) const {//lhc
        int lfx, rfx, lex, rex, lfy, rfy, ley, rey;
        lfx = (*left)[0]->x;
        lex = (*left)[left->size()-1]->x;
        rfx = (*right)[0]->x;
        rex = (*right)[right->size()-1]->x;
        lfy = (*left)[0]->y;
        ley = (*left)[left->size()-1]->y;
        rfy = (*right)[0]->y;
        rey = (*right)[right->size()-1]->y;

        if (left->size() != right->size()){
            return left->size() < right->size();
        }
        else{
            
            if ( lfx != rfx )
                return lfx < rfx;
            else{
                if ( lfy != rfy )
                    return lfy < rfy;
                else{
                    if ( lex != rex )
                        return lex < rex;
                    else
                        return ley < rey;
                }
            }
        }
    }
};

typedef set<Path*, LessPath>    PathSet;

void init(char *fn);

void BFS(Point* s);

bool point_isEqual(Point* point, Point* s);

bool findTerms_V (Point* point, PointSet* V);

void dump_to_file(char* file_name);

Path* maze(Point* s, Point* t);

Path* retrace(Point* s, Point* t);

void print_graph();

void print_path(Path* path);

void clean();

void prim();

void kruskal();

int find(int x);

void draw();

static void delay (void);

static void button_press (float x, float y);

static void drawscreen (void);

static void new_button_func (void (*drawscreen_ptr) (void)); 

double mazegetTime();
