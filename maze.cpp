#include <fstream>
#include <assert.h>
#include <sstream>
#include <iostream>
#include "maze.h"
#include <iomanip>
#include <sys/resource.h>
#include <math.h>
#include "graphics.h"
#include <cstdlib> 
#include <cstdio> 
#include <vector>
#include <queue>//lhc

using namespace std;

int **G;                    //the whole graph
Point* upperright;          //the upper right point of the graph
Path terms;                 //terminals to be connected
ObstacleVector obs;         //obstacles
PathVector completeGraph;   
PathVector mst;             //minimum spanning tree
int *father;                //father node

int main(int argc, char *argv[]) {
    
    cout << "Begin ..." << endl;
    init(argv[1]);
    

    //part 1: finish prim() of MST
    
    
    double beginTime1 = mazegetTime();
    prim();
    
    double endTime1 = mazegetTime();
    //print out MST
    
    cout << "Printing out MST ..." << endl;
    for (PathVector::iterator spi = mst.begin(); spi != mst.end(); spi++) {
        Path* sp = (*spi);
        for (Path::iterator pi = sp->begin(); pi != sp->end(); pi++) {
            G[(*pi)->x][(*pi)->y] = 1;
        }
    }

    //calculate wirelength
    int plen = 0;
    for (int i = 1; i <= upperright->x; i++)
        for (int j = 1; j <= upperright->y; j++)
            if(G[i][j] == 1)
                plen++;
    
    print_graph();
    // write function to dump you results into a file called 
    // "mst_result.txt"
    // the content is the same as the printed content in print_graph();
    char* file_name1 = "mst_result.txt";
    dump_to_file(file_name1);
    clean();
   

    // part 2: design your own routing algorithm


    double beginTime = mazegetTime();
    kruskal();

    double endTime = mazegetTime();

    //print out complete graph
    cout << "Printing out complete graph ..." << endl;
    for (PathVector::iterator spi = completeGraph.begin(); spi != completeGraph.end(); spi++) {
        Path* sp = (*spi);
        for (Path::iterator pi = sp->begin(); pi != sp->end(); pi++) {
            G[(*pi)->x][(*pi)->y] = 1;
        }
    }
    print_graph();

    //calculate wirelength
    int klen = 0;
    for (int i = 1; i <= upperright->x; i++)
        for (int j = 1; j <= upperright->y; j++)
            if(G[i][j] == 1)
                klen++;
    //dump results to file
    char* file_name2 = "routing_result.txt";
    dump_to_file(file_name2);
    clean();


    //cout << "Total wirelength:" << len << endl;
    cout << "Total runtime (part1):" << endTime1 - beginTime1 << " usec." << endl;
    cout << "Total runtime (part2):" << endTime - beginTime << " usec." << endl;


    cout << "Prim wirelength (part1):" << plen << endl;

    cout << "Kruskal wirelength (part2):" << klen << endl; 

    cout << "Finished." << endl;

    //draw();
}



void init(char *fn) {
    ifstream in;
    in.open(fn);
    if (!in.is_open()) {
        cerr << "Could not open file " << fn << endl;
        assert(0);
    }
    string line, word;

    while(true) {
        getline(in, line);
        if (in.eof()) break;
        if (line.length() < 2) continue; // empty line, skip
        if (line.c_str()[0] == '#') continue;

        istringstream is(line);
        // get token
        is >> word;
        if(!word.compare("grid")) {
            is >> word;
            upperright = new Point();
            sscanf(word.c_str(), "(%d,%d)", &upperright->x, &upperright->y);
            cout << "grid (" << upperright->x << "," << upperright->y << ")" << endl;
        }
        if(!word.compare("term")) {
            is >> word;
            Point* tp = new Point();
            sscanf(word.c_str(), "(%d,%d)", &tp->x, &tp->y);
            cout << "term (" << tp->x << "," << tp->y << ")" << endl;
            terms.push_back(tp);
        }
        if(!word.compare("obs")) {
            is >> word;
            Point* tp1 = new Point();
            Point* tp2 = new Point();
            sscanf(word.c_str(), "(%d,%d)", &tp1->x, &tp1->y);
            cout << "obs (" << tp1->x << "," << tp1->y << ")";
            is >> word;
            sscanf(word.c_str(), "(%d,%d)", &tp2->x, &tp2->y);
            cout << " (" << tp2->x << "," << tp2->y << ")" << endl;
            Obstacle* to = new Obstacle(tp1,tp2);
            obs.push_back(to);
        }

    }
    in.close();

    G = new int*[upperright->x + 2];
    //set boundary
    for (int i = 0; i < upperright->x + 2; i++) {
        G[i] = new int[upperright->y + 2];
    }

    clean();
    
    for ( int i = 0; i < upperright->x + 2; i++) {
        G[i][0] = -1;
        G[i][upperright->y + 1] = -1;
    }

    for ( int i = 0; i < upperright->y + 2; i++) {
        G[0][i] = -1;
        G[upperright->x + 1][i] = -1;
    }

    //set obstacles
    for(ObstacleVector::iterator oi = obs.begin(); oi != obs.end();
        oi++) {
        for (int i = (*oi)->ll->x; i <= (*oi)->ur->x; i++)
            for (int j = (*oi)->ll->y; j <= (*oi)->ur->y; j++)
                G[i][j] = -1;

    }

    for(Path::iterator tp = terms.begin(); tp != terms.end(); tp++) {
            (*tp)->paths = new PathVector();
    }
}



 
void BFS(Point* s){//lhc
    //TRACE
    int dir[4][2] = {0,1,0,-1,1,0,-1,0};

    queue<Point*> q;
    Point* cur = new Point();
    int cx,cy,nx,ny;

    int trace[upperright->x+2][upperright->y+2];

    for (int j = upperright->y; j > 0; j--) {
        for (int i = 1; i <= upperright->x; i++) {
            trace[i][j] = 0;
        }
    }
    trace[s->x][s->y]=1;
    q.push(s);

    while(!q.empty())
    {
        cur = q.front();
        cx = cur->x;
        cy = cur->y;

        q.pop();

        for(int i=0; i<4; i++)
        {
            nx = cx + dir[i][0];
            ny = cy + dir[i][1];

            if(trace[nx][ny] == 0 && G[nx][ny] > 0){
                
                trace[nx][ny] = trace[cx][cy] + 1;
                Point* nex = new Point(nx,ny);             
                q.push(nex);
            }
        }
    }

    //TRACE BACK
    Point* tp = new Point();
        

    int tx, ty, ntx, nty, nextemp;

    for(int i = 0; i < terms.size(); i++){

            Path *sPath = new Path();
            * tp = Point(terms[i]);
            sPath->push_back(terms[i]);

            while(!point_isEqual(tp,s))
            {
                for(int j = 0; j < 4; j++){
                    
                    tx = tp->x ;
                    ty = tp->y ;
                    ntx = tx + dir[j][0];
                    nty = ty + dir[j][1];
                    nextemp = trace[ntx][nty];
                
                    if (nextemp == trace[tx][ty]-1){

                        Point* tbp = new Point(ntx, nty);
                        sPath->push_back(tbp);
                    
                        tp->x = ntx;//move tp
                        tp->y = nty;
                        break;
                    }
                
                }            
            }
            s->paths->push_back(sPath);
    }

    //print s paths
    /*for (PathVector::iterator spi = s->paths->begin(); spi != s->paths->end(); spi++) {
        Path* sp = (*spi);
        for (Path::iterator pi = sp->begin(); pi != sp->end(); pi++) {
            cout << '(' << (*pi)->x << ','<< (*pi)->y << ')';
        }
        cout << endl;
    } */   

}

bool point_isEqual (Point* point, Point* s){//lhc
    int px,py;
    px = point->x;
    py = point->y;
    int sx,sy;
    sx = s->x;
    sy = s->y;
    if (px==sx && py==sy)
        return true;
    else
        return false;
}


bool findTerms_V (Point* point, PointSet* V){//lhc
    int px,py;
    px = point->x;
    py = point->y;

    for (PointSet::iterator ps = V->begin(); ps != V->end(); ps++) {
        int psx,psy;
        Point* psp = (*ps);
        psx = psp->x;
        psy = psp->y;
        if(px==psx && py==psy)
            return true;
    }
    return false;
}


int termIndex (Point* point){//lhc
    int px,py;
    px = point->x;
    py = point->y;

    int i=0;
    for (Path::iterator ps = terms.begin(); ps != terms.end(); ps++) {
        int psx,psy;
        Point* psp = (*ps);
        psx = psp->x;
        psy = psp->y;
        if(px==psx && py==psy)
            return i;
        i++;
    }
}


void prim(){
// find all paths
    for(int i = 0; i < terms.size(); i++)
        BFS(terms[i]);

    PointSet* U = new PointSet();
    PointSet* V = new PointSet();
    //move terms from U to V
    for(int i = 0; i != terms.size(); i++){
        U->insert(terms[i]);
    }
    V->insert(terms[0]);

    PathSet Edges[terms.size()];
    for(int i = 0; i != terms.size(); i++){
        for (PathVector::iterator spl = terms[i]->paths->begin(); spl != terms[i]->paths->end(); spl++) {
        Path* spm = (*spl);
        Edges[i].insert(spm); 
        }
    }  

    for(int i = 0; i != terms.size(); i++) {
        PathSet::iterator spj = Edges[i].begin();
        Edges[i].erase(*spj);
    }
/*for(int i = 0; i != terms.size(); i++) {
    for (PathSet::iterator spi = Edges[i].begin(); spi != Edges[i].end(); spi++) {
        Path* sp = (*spi);
        for (Path::iterator pi = sp->begin(); pi != sp->end(); pi++) {
            //[(*pi)->x][(*pi)->y] = 1;
            cout << "(" << (*pi)->x << "," << (*pi)->y << ") " ;
        }
        cout << endl;
    }
}*/

/*
    cout << "print set edges..." << endl;
    for(int i = 0; i != terms.size(); i++) {
    for (PathSet::iterator spj = Edges[i].begin(); spj != Edges[i].end(); spj++) {
        Path* sp1 = (*spj);
        //cout << sp1 -> size() << endl;
        for (Path::iterator pi2 = sp1->begin(); pi2 != sp1->end(); pi2++) {
            cout << '(' << (*pi2)->x << ','<< (*pi2)->y << ')';
        }
        cout << endl;
    } 
    }
*/    
    
    //prim
    PathSet mst_temp;
    int index;
    while( U->size() != V->size() ){
    // start with terms in V
        for(PointSet::iterator psp1 = V->begin(); psp1 !=V->end(); psp1++){
            Point* point_t = (*psp1);
            index = termIndex(point_t);
    //search mininal edges whose terminals do not belong to V
                for(PathSet::iterator spe = Edges[index].begin(); spe !=Edges[index].end(); spe++){
                    Path* min = (*spe);
                    Point* term = new Point((*min)[0]);
                    
    // if terminal doesn't belong to V, put this path into temp mst
                    if(!findTerms_V(term, V)){
                        mst_temp.insert(min);
                        break;
                    }
                    else{
                        Edges[index].erase(*spe);
                    }
            }
        }  
        
        if( mst_temp.size() != 0){
            // put the first(min) path into mst
                PathSet::iterator msp = mst_temp.begin();
                Path* msf= (*msp);
                mst.push_back(msf);
            // put terminal into V
                Point* termV = new Point((*msf)[0]);
                V->insert(termV);
            // clear mst_temp
                while(mst_temp.size() != 0){
                    PathSet::iterator msc = mst_temp.begin();
                    mst_temp.erase(*msc);
                }
        }                    
    }
}

int find(int x)  //²éÕÒ 
{
    if(x==father[x]) return father[x];
    return father[x]=find(father[x]);
}

void kruskal(){
// find all paths
    for(int i = 0; i < terms.size(); i++)
        BFS(terms[i]);

    PointSet* U = new PointSet();//move terms from U to V
    PointSet* V = new PointSet();    

    for(int i = 0; i != terms.size(); i++){
        U->insert(terms[i]);
    }

    PathSet Edges;
    for(int i = 0; i != terms.size(); i++){
        for (PathVector::iterator spl = terms[i]->paths->begin(); spl != terms[i]->paths->end(); spl++) {
        Path* spm = (*spl);
        Edges.insert(spm); 
        }
    }  

    for(int i = 0; i != terms.size(); i++) {
        PathSet::iterator spj = Edges.begin();
        Edges.erase(*spj);
    }
    
    //krustal
    father = new int[terms.size()];// father nodes
    for (int i = 0; i != terms.size(); i++){
        father[i] = i;
    }

    Path* mini = new Path();
        while ( Edges.size() != 0 ){
            PathSet::iterator spe = Edges.begin();
            mini = (*spe);
            Point* termb = new Point((*mini)[0]);
            Point* terme = new Point((*mini)[(mini->size())-1]);
            int fb = find(termIndex(termb));
            int fe = find(termIndex(terme));
        // different father node would avoid loop
            if(fb != fe){
                father[fb] = fe;
                completeGraph.push_back(mini);
                V->insert(termb);
                V->insert(terme);
            }
                    
            Edges.erase(*spe);                                         
        }
}


void print_path(Path* path) {
    for (Path::iterator pi = path->begin(); pi != path->end(); pi++) {
        cout << "(" << (*pi)->x << "," << (*pi)->y << ") " << endl;
    }
}

void print_graph() {
    for (int j = upperright->y; j > 0; j--) {
        for (int i = 1; i <= upperright->x; i++) {
            if(G[i][j] == MAX)
                cout << " N | ";
            else
                cout << setw(2) << G[i][j] << " | ";
        }
        cout << endl;
    }

}

void dump_to_file(char* file_name){
    ofstream fout;
    fout.open(file_name);
    if(!fout.is_open())
        {
           cerr << "Could not open file " << fout <<endl;
           exit(1);
        }
    // output graph
    for (int j = upperright->y; j > 0; j--) {
        for (int i = 1; i <= upperright->x; i++) {
            if(G[i][j] == MAX)
                fout << " N | ";
            else
                fout << setw(2) << G[i][j] << " | ";
        }
        fout << endl;
    }
/*    // output wirelength
    int len = 0;
    for (int i = 1; i <= upperright->x; i++)
        for (int j = 1; j <= upperright->y; j++)
            if(G[i][j] == 1)
                len++;
    fout << "Total wirelength:" << len << endl;
    //cout << "Total runtime:" << endTime - beginTime << " usec." << endl;

    fout << "Finished." << endl;
*/    
    fout.close();

}

void clean() {
    for (int i = 1; i <= upperright->x; i++)
        for (int j = 1; j <= upperright->y; j++)
            if (G[i][j] != -1)
                G[i][j] = MAX;
}


void draw() {
/* initialize display */
 init_graphics("Multiple terminals routing");


 init_world (0.,0.,1000.,1000.);
 //update_message("Interactive graphics example number 2!.");
 //create_button ("Window", "Next", new_button_func);
 drawscreen(); 
 event_loop(button_press, drawscreen);    
    
}

static void drawscreen (void) {

/* redrawing routine for still pictures.  Graphics package calls  *
 * this routine to do redrawing after the user changes the window *
 * in any way.                                                    */

 clearscreen();  /* Should be first line of all drawscreens */
 setfontsize (10);
 setlinestyle (SOLID);
 setlinewidth (0);
 setcolor (BLACK);
 int windowsize=WINDOWSIZE;
 int gridsize=(windowsize/upperright->x > windowsize/upperright->y)?
        (windowsize/upperright->y*.8):(windowsize/upperright->x*.8);

 //grid
 for (int i = 0; i < upperright->x; i++) {
    drawline(gridsize+i*gridsize, windowsize-gridsize, gridsize+i*gridsize, windowsize-upperright->y*gridsize);
 }

 for (int i = 0; i < upperright->y; i++) {
    drawline(gridsize, windowsize-gridsize-i*gridsize, upperright->x*gridsize, windowsize-gridsize-i*gridsize);
 }

 //obstacles
 for (ObstacleVector::iterator oi = obs.begin(); oi != obs.end(); oi++) {
    setcolor(RED);
    fillrect((*oi)->ll->x*gridsize, windowsize-(*oi)->ll->y*gridsize, (*oi)->ur->x*gridsize+gridsize, windowsize-(*oi)->ur->y*gridsize-gridsize);
 }   
 
 //MST
 for (PathVector::iterator spi = mst.begin(); spi != mst.end(); spi++) {
    Path* path = *spi;
    for (Path::iterator pi = path->begin(); pi != path->end(); pi++) {
        setcolor(BLUE);
        fillrect((*pi)->x*gridsize, windowsize-(*pi)->y*gridsize, (*pi)->x*gridsize+gridsize, windowsize-(*pi)->y*gridsize-gridsize);
        flushinput();
    }
 }

 //terminals
 for (Path::iterator pi = terms.begin(); pi != terms.end(); pi++) {
    setcolor(YELLOW);
    fillrect((*pi)->x*gridsize, windowsize-(*pi)->y*gridsize, (*pi)->x*gridsize+gridsize, windowsize-(*pi)->y*gridsize-gridsize);
    flushinput();
    //delay();
 } 

}

static void delay (void) {

/* A simple delay routine for animation. */

 int i, j, k, sum;

 sum = 0;
 for (i=0;i<50000;i++) 
    for (j=0;j<i;j++)
       for (k=0;k<30;k++) 
          sum = sum + i+j-k; 
}

static void button_press (float x, float y) {

/* Called whenever event_loop gets a button press in the graphics *
 * area.  Allows the user to do whatever he/she wants with button *
 * clicks.                                                        */
 int windowsize=WINDOWSIZE;
 int gridsize=(windowsize/upperright->x > windowsize/upperright->y)?
        (windowsize/upperright->y*.8):(windowsize/upperright->x*.8);
 int ax = int(x) / gridsize;
 int ay = (windowsize - int(y)) / gridsize;

 printf("User clicked a button at grid (%d,%d)\n", ax, ay);
}


static void new_button_func (void (*drawscreen_ptr) (void)) {

 printf ("You pressed the new button!\n");
 setcolor (MAGENTA);
 setfontsize (12);
 drawtext (500., 500., "You pressed the new button!", 10000.);
}

double mazegetTime() {
    struct rusage r;
    getrusage(RUSAGE_SELF, &r);
    
    return static_cast<double>(r.ru_utime.tv_sec)*1000000 + static_cast<double>(r.ru_utime.tv_usec);
}
