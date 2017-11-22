#include "blob.h"
#include "mbed.h"
#include "misc.h"

extern Serial pc;
// Randomly initialize a blob's position, velocity, color, radius, etc.
// Set the valid flag to true and the delete_now flag to false.
// delete_now is basically the derivative of valid. It goes true for one
// fram when the blob is deleted, and then it is reset to false in the next frame
// when that blob is deleted.
void BLOB_init(BLOB* b) {
    /*
    float old_x, old_y;
    float posx, posy;
    float vx, vy;
    float rad;
    int color;    
    bool valid;
    bool delete_now;*/
    int x= rand() % 256;
    int y= rand() % 256;
    b->old_x= x;
    b->old_y= y;
    b->posx= x;
    b->posy= y;
    b->vx= (50);
    b->vy= (50);
    b->rad=8;
    b->color=0xFEF078;
    b->valid= true;
    b->delete_now= false;    
}
// Take in a blob and determine whether it is inside the world.
// If the blob has escaped the world, put it back on the edge
// of the world and negate its velocity so that it bounces off
// the boundary. Use WORLD_WIDTH and WORLD_HEIGHT defined in "misc.h"
void BLOB_constrain2world(BLOB* b) {
    // ***
    float px= (b->posx);
    float py= (b->posy);
    

       
    float velx=b->vx;
    float vely=b->vy;
    
    if ((px)>0){
        float p= 256- (b->rad);
            if (px>=p){
              b->posx= p;
              b->vx=-velx;
              }      
        }
    else if ((px)<0){
         float n= -256+ (b->rad);
         if (px<=n){
            b->posx=n;
            b->vx=-velx;
            }   
        }
    
   if ((py)>0){
        float p= 256- (b->rad);
            if (py>=p){
              b->posy= p;
              b->vy=-vely;
              }      
        }
    else if ((py)<0){
         float n= -256+ (b->rad);
         if (py<=n){
            b->posy=n;
            b->vy=-vely;
            }   
        }        
              

}
// Randomly initialize a blob. Then set the radius to the provided value.
void BLOB_init(BLOB* b, int rad) {
    int x= rand() % 256;
    int y= rand() % 256;
    b->old_x= x;
    b->old_y= y;
    b->posx= x;
    b->posy= y;
    b->vx= (9000/(b->rad));
    b->vy= (9000/(b->rad));
    b->rad=rad;
    b->color=0xff4444;
    b->valid= true;
    b->delete_now= false; 
}
// Randomly initialize a blob. Then set the radius and color to the 
// provided values.
void BLOB_init(BLOB* b, int rad, int color) {
    // ***
    int x= rand() % 256;
    int y= rand() % 256;
    b->old_x= x;
    b->old_y= y;
    b->posx= x;
    b->posy= y;
    b->vx= (9000/(b->rad)); 
    b->vy= (9000/(b->rad));
    b->rad=rad;
    b->color=color;
    b->valid= true;
    b->delete_now= false; 
}
// For debug purposes, you can use this to print a blob's properties to your computer's serial monitor.
void BLOB_print(BLOB b) {
    pc.printf("(%f, %f) <%f, %f> Color: 0x%x\n", b.posx, b.posy, b.vx, b.vy, b.color);
}
// Return the square of the distance from b1 to b2
float BLOB_dist2(BLOB b1, BLOB b2) {
       
   /* float posx, posy;
    */   
    float dist= (b2.posx-b1.posx)*(b2.posx-b1.posx)+(b2.posy-b1.posy)*(b2.posy-b1.posy);
    return (dist);
}