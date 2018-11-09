#include "mbed.h"
#include "RGBmatrixPanel.h" // Hardware-specific library

//DigitalOut gpo(D0);
//DigitalOut led(LED_RED);

//structs/types
typedef struct point{
    int x;//index from MSB
    int y;//index of array
} point;

typedef struct node{
    point value;
    struct node * next;
} node_t;

//global vars
node_t * qBFS;
node_t * qDFS;

PinName ub1=PTC7;//blue 1
PinName ug1=PTC2;//green1 was ptc3
PinName ur1=PTC0;//red1 was ptc5
PinName lb2=PTC9;
PinName lg2=PTC3;//green2 was ptc2
PinName lr2=PTC5;//red2 was ptc0
PinName a = PTC8;
PinName b = PTC4;
PinName c = PTC1;
PinName d = PTA1;
PinName sclk = PTC11;
PinName lat = PTC16;
PinName oe = PTC10;
//+3 grounds

//declare matrix
RGBmatrixPanel matrix(ur1,lr2,ug1,lg2,ub1,lb2,a,b,c,d,sclk,lat,oe,false);
uint32_t red[32] = {};//red pixel locations
uint32_t green[32] = {};//green pixel locations


uint32_t maze_1[32] = {0xFFFF7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFC7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF1FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFC7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF1FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFC7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF1FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFC7FFF,
0xFFFF7FFF,0xFFFF7FFF,0xFFFF1FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF7FFF,0xFFFF7FFF};
uint32_t maze_2[32] = {0xFFFF7FFF,0xD55000A9,0x800557AB,0xDDFDD401,
0xF70111FF,0x85FDD485,0xD00195BD,0xFAECDC01,
0x9F8E89AB,0xD528DB2B,0x806A4979,0xEFCB5B73,
0xC81B7E27,0x9FD2177D,0xD01B4201,0x9FD9D7AB,
0xD0131139,0x9FB9BB8F,0xF90C8F21,0xADA9AC7B,
0x802CB921,0xBFBE937D,0xA00ABA05,0xBED8935F,
0xB09AD941,0x96D253D5,0xBF9AD61D,0x90329FD1,
0xBE9AC31D,0x90CE6A49,0xBA58CB5D,0xBFFFFFFF};
uint32_t maze_3[32] ={0xFFFF7FFF,0xA0002A81,0xBFEA8A2F,0xA08BA0FD,
0xBEFA2A55,0xAAD3AAC1,0x8006BE15,0xFF7203BF,
0x803BAE01,0xDB61E75F,0x9E7B0DD1,0xB0C3E907,
0xBA6E83D5,0xA2FC3711,0xBB8161D5,0xA0AD2F15,
0xB7FF65FD,0x94003059,0xDFDEBB0D,0x9010A1B9,
0xFF5EACF3,0x8048AE27,0xAAFEA4B1,0xFF82B69B,
0x9028F4C9,0xC77E169D,0xEDD0DCBF,0x89058EAD,
0xFDFDDB85,0xA0D080AD,0x8A05D5A1,0xBFFFFFFF};
uint32_t maze_4[32] = {0xFFFF7FFF,0xA30D2001,0xB9E06DBB,0xAD7F0F89,
0x800078DD,0xEFAF1247,0x88384753,0xDF62D1D9,
0x903EF47D,0xDEA086D7,0xF4AEAC01,0x81F8A76F,
0xF503AC21,0xA5F9276B,0x8F83B249,0xD83B36DB,
0xCD6263C9,0x9FFB377B,0xB8039013,0xAF69D6F9,
0xAA4B5C0B,0x8AEE0569,0xA88BDF6B,0xAEE80A29,
0xA48FDBAD,0xADE41AE5,0xE5BFC04F,0x8D00157D,
0xA7DFD501,0xF4101DDD,0x855AC845,0xBFFFFFFF};
uint32_t maze_5[32] = {0xFFFF7FFF,0x862032A9,0xF3BF662D,0xDB1434E5,
0x804181AD,0xFB7B2C05,0xA941A551,0x83F537D5,
0xFA85A157,0x822CF571,0xFFA7D725,0x8034006D,
0xFF9FDAFD,0x80381205,0xFE93BBD7,0x90BA8851,
0xDDAE3DFF,0x81209855,0xFBADCAD1,0xAABF5A07,
0x823412DD,0xAAA1B68D,0xBEFBFEE1,0x9021004B,
0xB6FBAED9,0xA441F8F3,0xBF575247,0xA05C06F1,
0xF751F415,0xC47F85B5,0x96C03515,0xBFFFFFFF};
uint32_t maze_6[32] = {0xFFFF7FFF,0xAAAA5501,0x800B057F,0xBFA05015,
0xA03576B5,0xFFE52781,0x800FB0FB,0xFFD81ADF,
0x800F5A15,0xF5644B41,0x85FFD955,0xFF260B55,
0xDB8CDA7F,0x8021CB35,0xB7FC5F81,0xFC01C22F,
0xD1BC6EA1,0x851142AD,0xEFFDEAB9,0x88016AAD,
0xEFBC2A85,0x88357FDF,0xEBE1DB01,0x8E0C01AB,
0xA4BDACFF,0xFFA13DC5,0x802DB011,0xAAB89ABF,
0xBF92CAA9,0xAC3E9B8B,0xA160C921,0xBFFFFFFF};
uint32_t maze_7[32] = {0xFFFF7FFF,0xC0000101,0x9D556C77,0xD1F7255D,
0x95046D09,0xDFFF2563,0xF2007F49,0x9BF5026D,
0xD1856BE9,0x8437283D,0xFFA27AF5,0x803B1205,
0xABA17755,0xFA35225D,0x837DB747,0xFA409351,
0x9AEAD955,0xC24ECBDD,0x97DC6905,0xF411EDD7,
0x86DC4C91,0xB691E9DB,0xE4DC8C53,0x8D91DED9,
0xA8BC904B,0xADE9D6E9,0xE883142F,0x8EF9DEA3,
0xBA430AA9,0xA0F9D8AF,0xAA830DA1,0xBFFFFFFF};
uint32_t maze_8[32] = {0xFFFF7FFF,0xC0000101,0x9D556C77,0xD1F7255D,
0x95046D09,0xDFFF2563,0xF2007F49,0x9BF5026D,
0xD1856BE9,0x8437283D,0xFFA27AF5,0x803B1205,
0xABA17755,0xFA35225D,0x837DB747,0xFA409351,
0x9AEAD955,0xC24ECBDD,0x97DC6905,0xF411EDD7,
0x86DC4C91,0xB691E9DB,0xE4DC8C53,0x8D91DED9,
0xA8BC904B,0xADE9D6E9,0xE883142F,0x8EF9DEA3,
0xBA430AA9,0xA0F9D8AF,0xAA830DA1,0xBFFFFFFF};
uint32_t maze_9[32] = {0xFFFF7FFF,0x81003353,0xAC7E9819,0xF940C343,
0xD3FE5757,0x8700D153,0xF45E5759,0x8750F24B,
0xEC7EC77F,0x8F409309,0xE5F5B67B,0x8D051701,
0xA857BC6B,0xEF5C06C9,0x8877AFEB,0xEB402D3B,
0x8FF6E061,0xFAA48F7B,0x800DE521,0xFFE70DFB,
0xC00C6019,0x97EF4BB3,0xD40468FB,0xFF56CBD1,
0xD672F815,0x80C6837D,0xBA93F907,0xA3B60351,
0xBF13FB75,0xF05EA115,0x85500B55,0xBFFFFFFF};
uint32_t maze_10[32] = {0xFFFF7FFF,0xBA880809,0x90E37BBB,0xDEBA0AE1,
0x80AEA00B,0xBA00AD59,0xA2DDA74B,0xBB91315B,
0xEF3B7749,0xC07325FF,0x97C670B5,0xDC14FA01,
0x915EA2D5,0xFFC43A95,0x801F62D5,0xB5B42BDD,
0xA7916F17,0xFE3B2841,0x80A27F6B,0xEFB6C82B,
0xB8221F69,0xAD6ECA4B,0x85C268EF,0xB076CA81,
0xF5C26EED,0x8476C48D,0xEEC26DE7,0x8AF74F0D,
0xD8246861,0x9B773EF5,0xC9416045,0xDFFFFFFF};

uint32_t * current_maze = NULL;//pointer to which maze to run
int dir=2; //0 = north, 1 = east, 2 = south, 3 = west
point currentBFS = {16, 0};
point currentDFS = {16, 0};
int visitedBFS[1024]={-1};//mark end of list
int visitedDFS[1024]={-1};//mark end of list
int bfsDone = 0;
int dfsDone = 0;
int numMazes = 10;//number of mazes to cycle through

//---\\

void delay(int x){
     for(int i=0;i<x;i++){
          wait_us(10);
          matrix.updateDisplay();//be careful about the timing of this
     }
}

//precondition pos must be 0-31
int getbit(uint32_t num, int pos){
    pos = 31 - pos;//32 gets you -1
    return (num & ( 1 << pos )) >> pos;
}

//setbit returns a new number after having set the posth' bit of the input num to bit
int setbit(int num, int pos, int bit){
    int temp = 1 << pos;
    //if it's 0, if it's 1
    return (bit == 0) ? (num &= !temp) : (num |= temp);
}

//@param  matx is a length 32 array of integers (precondition)
//@param  color 16bit color 0x0000 ~ 0xFFFF
void drawMatrix(uint32_t matx[], int16_t color){
    for(int y = 0; y < 32; y++){//loop over ints
        for(int x = 0; x < 32; x++){//loop over bits
            if(getbit(matx[y], x) > 0){
                  matrix.drawPixel(x, y, color);
            }
            //else matrix.drawPixel(x, y, 0x0000);//this had 0x000 before

        }
    }
}

//@param colormode: 0 is maze, 1 is BFS, 2 is DFS
//visit_set_color is called from visit to set the color of a pixel
//depending on which traversal is accessing the pixel
void visit_set_color(uint16_t x, uint16_t y, int isBFS){
   if(isBFS) {
     //green[y] = setbit(green[y],x,1);
     green[y] |= (1 << (31 - x));
   }
   else{
     //red[y] = setbit(red[y],x,1);
     red[y] |= (1 << (31 - x));
   }

}

//when visiting a pixel, paint it the proper color
void paintPixel(uint32_t x, uint32_t y){
    //retrieve its current color
    uint16_t r = getbit(red[y], x)*0xF800;
    uint16_t g = getbit(green[y], x)*0x07E0;
    uint16_t b = getbit(current_maze[y], x)*0x001F;

    //draw the pixel as the combined color
    uint16_t color = r + g + b;
    matrix.drawPixel(x, y, color);
}

//drawMatrix for multiple colors
//gets color information from the red, green, blue matrices
void repaint(){
    for(int y = 0; y < 32; y++){//loop over ints
        for(int x = 0; x < 32; x++){//loop over bits
             paintPixel(x, y);
        }
    }
}


/*
    adds elt to end of queue
    queue = 0 is qDFS
    queue = 1 is qBFS

*/
void enq (node_t * elt, int isBFS) {
    node_t * tmp = (isBFS) ? qBFS : qDFS;
    if(tmp == NULL) {
        if (isBFS){
          qBFS = elt;
        }
        else{
          qDFS = elt;
        }
    }
    else {
        while(tmp->next != NULL) {
             tmp = tmp->next;
         }
        tmp->next = elt;
    }
}

node_t * deq () {
    node_t * tmp = qBFS;
    if(tmp == NULL) {
        return NULL;
    }
        qBFS = qBFS->next;
        tmp->next = NULL;
    return tmp;
}

node_t * pop(int queue){
    node_t * q;
    node_t * tmp;

    q = (queue) ? qBFS : qDFS;

    printf("%s\n", "pop");
    if(q == NULL) return NULL;

    if(q->next == NULL){
        tmp = q;
        q = NULL;
        return tmp;
    }

    while(q->next->next != NULL) {
        q = q->next;
    }
    //q is second to last elt
    tmp = q->next; //tmp is last elt
    q->next = NULL;

    return tmp;
}

int hasBeenVisited(point cell, int isBFS){
    if (isBFS){
        for (int i=0; visitedBFS[i] != -1; i++){
            if (visitedBFS[i] == (cell.x + cell.y*32)) return 1;
        }
    }
    else{
        for (int i=0; visitedDFS[i] != -1; i++){
            if (visitedDFS[i] == (cell.x + cell.y*32)) return 1;
        }
    }
    return 0;
}

/*Returns the list of current's neighboring cells as coordinates.*/
point* getNeighbors(int isBFS){//returns an array of points
    point pt = (isBFS) ? currentBFS : currentDFS;
    static point n[4];
     //n[i] is pointer to point or null

    if (pt.y - 1 >= 0 ){
        n[0] = (getbit(current_maze[pt.y - 1], pt.x) == 0) ? ((point) {pt.x, pt.y - 1}) : (point){};
    }else n[0] = (point){};

    n[1] = ((pt.x + 1 <= 31) && (getbit(current_maze[pt.y], pt.x + 1) == 0)) ? ((point) {pt.x + 1, pt.y}) : (point){};


    if (pt.y + 1 <= 31 ){
        n[2] = (getbit(current_maze[pt.y + 1], pt.x) == 0) ? ((point) {pt.x, pt.y + 1}) : (point){};
    }else n[2] = (point){};

    n[3] = ((pt.x - 1 >= 0 ) && (getbit(current_maze[pt.y], pt.x - 1) == 0)) ? ((point) {pt.x - 1, pt.y}) : (point){};

    for (int i = 0; i < 4; i++){
        if (hasBeenVisited(n[i], isBFS)) n[i] = (point){}; //n should contain only unvisited cells
    }

    return n;//returns pointer to n[0]
}

//visit for multiple colors
void visitColor(point cell, int isBFS){
    visit_set_color(cell.x, cell.y, isBFS);

    if(isBFS){
        //not sure if we will need this if we're using rgb arrays
        int i = 0;
        while (visitedBFS[i] != -1) i++;//find end of list
        visitedBFS[i] = cell.x + 32*cell.y;//add cell
        visitedBFS[i+1] = -1; //update end
    }
    else{
        int j = 0;
        while (visitedDFS[j] != -1) j++;//find end of list
        visitedDFS[j] = cell.x + 32*cell.y;//add cell
        visitedDFS[j+1] = -1; //update end
    }
}

int stepBFS(){//TODO add Matias' fix for the last pixel
    currentBFS = deq()->value;//dequeue returns node and we want its value (point)
    point *neighbors = getNeighbors(1);
    //visit(current, 1);
    visitColor(currentBFS, 1);
    if (currentBFS.y == 31) return 1;
    for (int i=0; i < 4; i++){
        if((*(neighbors+i)).x){
            node_t * temp = (node_t *) malloc(sizeof(node_t));
            //node_t * temp = malloc(sizeof(node_t));
            temp->value = *(neighbors+i);
            temp->next = NULL;
            enq(temp,1);
        }
    }

    return 0;
}

int stepDFS(){
    currentDFS = pop(0)->value;
    point *neighbors = getNeighbors(0);
    visitColor(currentDFS, 0);
    if (currentDFS.y == 31) return 1;
    for (int i=0; i < 4; i++){
        if((*(neighbors+i)).x){
            node_t * temp = (node_t *) malloc(sizeof(node_t));
            temp->value = *(neighbors+i);
            temp->next = NULL;
            enq(temp,0);
        }
    }
    return 0;
}

//wipes an intarray
void wipe(uint32_t matx[]){
    for(int i = 0; i<32; i++){
        matx[i] = 0;
    }
}

void resetVisited(int matx[]){
    matx[0] = -1;
    for(int i = 1; i<1024; i++){
        matx[i] = 0;
    }
}

//We would have to hard code all of the mazes to cycle through
void selectMaze(int selector){
    //clear red, green, blue, reset current
    free(qBFS);
    free(qDFS);
    wipe(red);
    wipe(green);
    resetVisited(visitedBFS);
    resetVisited(visitedDFS);
    currentBFS = (point){16,0};
    currentDFS = (point){16,0};

    switch(selector){
     case 0:
        current_maze = maze_1;
        break;
     case 1:
        current_maze = maze_2;
        break;
     case 2:
        current_maze = maze_3;
        break;
     case 3:
        current_maze = maze_4;
        break;
     case 4:
        current_maze = maze_5;
        break;
     case 5:
        current_maze = maze_6;
        break;
     case 6:
        current_maze = maze_7;
        break;
     case 7:
        current_maze = maze_8;
        break;
     case 8:
        current_maze = maze_9;
        break;
     case 9:
        current_maze = maze_10;
        break;
    }
    //setup BFS/DFS stuff
    qBFS = (node_t *) malloc(sizeof(node_t));
    qBFS->value = currentBFS;
    qBFS->next = NULL;

    qDFS = (node_t *) malloc(sizeof(node_t));
    qDFS->value = currentDFS;
    qDFS->next = NULL;
}

int main(){
    current_maze = maze_1;
    qBFS = (node_t *) malloc(sizeof(node_t));
    qBFS->value = currentBFS;
    qBFS->next = NULL;

    qDFS = (node_t *) malloc(sizeof(node_t));
    qDFS->value = currentDFS;
    qDFS->next = NULL;

    int select = 0;
    while(1){
        if(!bfsDone) bfsDone = stepBFS();//bfs is green
        if(!dfsDone) dfsDone = stepDFS();

        repaint();//repaint will always repaint the maze_blue (blue)
        if(bfsDone && dfsDone){
            select++;
            select %= numMazes;
            selectMaze(select);
            bfsDone = !bfsDone;
            dfsDone = !dfsDone;
        }
        delay(1000);
    }
}
