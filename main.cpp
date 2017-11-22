// Student Side Shell Code
// For the baseline, anywhere you see ***, you have code to write.

#include "mbed.h"

#include "SDFileSystem.h"
#include "wave_player.h"
#include "game_synchronizer.h"
#include "misc.h"
#include "blob.h"
#include "playSound.h"
#include "RGBLed.h"

#define NUM_BLOBS 22
#define EASY 0
#define MEDIUM 1
#define HARD 2



DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

DigitalIn pb_u(p21);                        // Up Button
DigitalIn pb_r(p22);                        // Right Button
DigitalIn pb_d(p23);                        // Down Button
DigitalIn pb_l(p24);                        // Left Button

RGBLed myRGBLed(p12,p13,p14);               //RGB LED Object

Serial pc(USBTX, USBRX);                    // Serial connection to PC. Useful for debugging!
MMA8452 acc(p28, p27, 100000);              // Accelerometer (SDA, SCL, Baudrate)
uLCD_4DGL uLCD(p9,p10,p11);                 // LCD (tx, rx, reset)
SDFileSystem sd(p5, p6, p7, p8, "sd");      // SD  (mosi, miso, sck, cs)
AnalogOut DACout(p18);                      // speaker
wave_player player(&DACout);                // wav player
GSYNC game_synchronizer;                    // Game_Synchronizer
GSYNC* sync = &game_synchronizer;           //
Timer frame_timer;                          // Timer

Timer t;

int score1 = 0;                             // Player 1's score.
int score2 = 0;                             // Player 2's score.
char uwin[10]=("YOU WIN");
char ulose[10]=("YOU LOSE");

int difficulty;
int pwcount=3;
float timcnt=50;

char oink[] = "/sd/wavfiles/blip.wav";
char blip[] = "/sd/wavfiles/blip.wav";
char bep[] = "/sd/wavfiles/pep.wav";
char BUZZ[] = "/sd/wavfiles/BUZZ.wav";
// ***
// Display a pretty game menu on the player 1 mbed. 
// Do a good job, and make it look nice. Give the player
// an option to play in single- or multi-player mode. 
// Use the buttons to allow them to choose.
int game_menu(void) {
    uLCD.background_color(BGRD_COL);
    //uLCD.textbackground_color(BGRD_COL);
    uLCD.cls();
    uLCD.text_mode(TRANSPARENT);
    uLCD.filled_circle(90, 20 , 30, 0x990000);
    uLCD.filled_circle(20, 40, 15, 0xFF66CC);  
    
    uLCD.locate(1, 1);
    uLCD.text_width(2); 
    uLCD.text_height(4);
    uLCD.color(WHITE);
    uLCD.printf("sAGAR.gt");
    
 
    uLCD.filled_rectangle(4, 56, 124 , 124, 0x000000);
    uLCD.filled_rectangle(8, 60, 120 , 120, 0x00fcff);
    uLCD.filled_rectangle(12, 64, 116 , 116, 0x051249);   
    
    uLCD.set_font(FONT_7X8);
    uLCD.textbackground_color(0x051249);
    uLCD.text_width(1); 
    uLCD.text_height(1);
    
    uLCD.locate(6, 9);
    uLCD.set_font_size('2', '2');
    uLCD.text_bold(1);
    uLCD.puts("PBU: 1P");   
    
    uLCD.locate(6, 12);
    uLCD.set_font_size('2', '2'); 
    uLCD.text_bold(1);
    uLCD.puts("PBR: 2P"); 
    // ***
    // Spin until a button is pressed. Depending which button is pressed, 
    // return either SINGLE_PLAYER or MULTI_PLAYER.
     
   while(1) {
        if(!pb_u) { 
            pc.printf("SINGLE PLAYER SELECTED\n");
            return SINGLE_PLAYER;
        }
        if(!pb_r) {
            pc.printf("MULTI PLAYER SELECTED");
            return MULTI_PLAYER;    
        }
    }
    
}

int difficulty_menu(void) {
    //I created a submenu which allows user to choose difficulty level each corresponding to different AIs for Blob 1.
    uLCD.background_color(BGRD_COL);
    //uLCD.textbackground_color(BGRD_COL);
    uLCD.cls();
    uLCD.text_mode(TRANSPARENT);
    uLCD.filled_circle(90, 20 , 30, 0x990000);
    uLCD.filled_circle(20, 40, 15, 0xFF66CC);  
    
    uLCD.locate(1, 1);
    uLCD.text_width(2); 
    uLCD.text_height(4);
    uLCD.color(WHITE);
    uLCD.printf("Choose");
    
 
    uLCD.filled_rectangle(4, 56, 124 , 124, 0x000000);
    uLCD.filled_rectangle(8, 60, 120 , 120, 0x00fcff);
    uLCD.filled_rectangle(12, 64, 116 , 116, 0x051249);   
    
    uLCD.set_font(FONT_7X8);
    uLCD.textbackground_color(0x051249);
    uLCD.text_width(1); 
    uLCD.text_height(1);
    
    uLCD.locate(4, 9);
    uLCD.set_font_size('2', '2');
    uLCD.text_bold(1);
    uLCD.puts("PBU: EASY");   
    
    uLCD.locate(4, 11);
    uLCD.set_font_size('2', '2'); 
    uLCD.text_bold(1);
    uLCD.puts("PBR: MEDIUM"); 
    
    uLCD.locate(4, 13);
    uLCD.set_font_size('2', '2'); 
    uLCD.text_bold(1);
    uLCD.puts("PBD: HARD"); 
    // ***
    // Spin until a button is pressed. Depending which button is pressed, 
    // return either SINGLE_PLAYER or MULTI_PLAYER.
     
   while(1) {
        if(!pb_u) { 
            pc.printf("EASY SELECTED\n");
            return EASY;
        }
        if(!pb_r) {
            pc.printf("MEDIUM SELECTED");
            return MEDIUM;    
        }
        if(!pb_d) {
            pc.printf("HARD SELECTED");
            return HARD;    
        }
    }
    
}


// Initialize the game hardware. 
// Call game_menu to find out which mode to play the game in (Single- or Multi-Player)
// Initialize the game synchronizer.
void game_init(void) {
    
    led1 = 0; led2 = 0; led3 = 0; led4 = 0;
    
    pb_u.mode(PullUp);
    pb_r.mode(PullUp); 
    pb_d.mode(PullUp);    
    pb_l.mode(PullUp);
    
    pc.printf("\033[2J\033[0;0H");              // Clear the terminal screen.
    pc.printf("I'm alive! Player 1\n");         // Let us know you made it this far

    // game_menu MUST return either SINGLE_PLAYER or MULTI_PLAYER
    int num_players = game_menu();
    if (num_players==SINGLE_PLAYER){
    difficulty= difficulty_menu();
    }
    GS_init(sync, &uLCD, &acc, &pb_u, &pb_r, &pb_d, &pb_l, num_players, PLAYER1); // Connect to the other player.
        
    pc.printf("Initialized...\n");              // Let us know you finished initializing.
    srand(time(NULL));                          // Seed the random number generator.

    GS_cls(sync, SCREEN_BOTH);
    GS_update(sync);
}

// Display who won!
void game_over(int winner) {
    uLCD.color(BLUE);
    uLCD.text_height(4);
    if(SINGLE_PLAYER == sync -> play_mode){
        if (winner==WINNER_P1){
           /*GS_filled_rectangle (sync, SCREEN_P1, -64, 64, 64, -64, RED);    
           GS_textbackground_color (sync, SCREEN_P1, RED);
           GS_locate(sync, SCREEN_P1, -3, 2);
           GS_puts(sync, SCREEN_P1, uwin, 10);*/
           uLCD.text_mode(TRANSPARENT);
           uLCD.filled_rectangle(1, 1, 124 , 124, RED);
           uLCD.text_width(2); 
           uLCD.locate( 1,1);
           uLCD.puts(uwin);
        }
        else if (winner==WINNER_P2){
           /*GS_filled_rectangle (sync, SCREEN_P1, -64, 64, 64, -64, RED);    
           GS_textbackground_color (sync, SCREEN_P1, RED);
           GS_locate(sync, SCREEN_P1, -3, 2);
           GS_puts(sync, SCREEN_P1,ulose, 10);*/
           uLCD.text_mode(TRANSPARENT);
           uLCD.filled_rectangle(1, 1, 124 , 124, RED);
           uLCD.text_width(2); 
           uLCD.locate(1, 1);
           uLCD.puts(ulose);
           
           
        }
    }
    else if(MULTI_PLAYER == sync -> play_mode){
        if (winner==WINNER_P1){
           uLCD.text_mode(TRANSPARENT);
           uLCD.filled_rectangle(1, 1, 124 , 124, RED);
           uLCD.text_width(2); 
           uLCD.locate( 1,1);
           uLCD.puts(uwin);
           /*GS_filled_rectangle (sync, SCREEN_P1, -64, 64, 64, -64, RED);    
           GS_textbackground_color (sync, SCREEN_P1, RED);
           GS_locate(sync, SCREEN_P1, -3, 2);
           GS_puts(sync, SCREEN_P1, uwin, 10);
           GS_filled_rectangle (sync, SCREEN_P2, -64, 64, 64, -64, RED);    
           GS_textbackground_color (sync, SCREEN_P2, RED);
           GS_locate(sync, SCREEN_P2, -3, 2);
           GS_puts(sync, SCREEN_P2,ulose, 10);*/
        }
        else if (winner==WINNER_P2){
           uLCD.text_mode(TRANSPARENT);
           uLCD.filled_rectangle(1, 1, 124 , 124, RED);
           uLCD.text_width(2); 
           uLCD.locate( 1,1);
           uLCD.puts(uwin);
           /*GS_filled_rectangle (sync, SCREEN_P1, -64, 64, 64, -64, RED);    
           GS_textbackground_color (sync, SCREEN_P1, RED);
           GS_locate(sync, SCREEN_P1, -3, 2);
           GS_puts(sync, SCREEN_P1,ulose, 10);
           GS_filled_rectangle (sync, SCREEN_P2, -64, 64, 64, -64, RED);    
           GS_textbackground_color (sync, SCREEN_P2, RED);
           GS_locate(sync, SCREEN_P2, -3, 2);
           GS_puts(sync, SCREEN_P2, uwin, 10);*/
        }
    }
    // Pause forever.
    while(1);
}

// Take in a pointer to the blobs array. Iterate over the array
// and initialize each blob with BLOB_init(). Set the first blob to (for example) blue
// and the second blob to (for example) red. Set the color(s) of the food blobs however you like.
// Make the radius of the "player blobs" equal and larger than the radius of the "food blobs".
void generate_blobs(BLOB* blobs) {
    // ***
    BLOB_init(blobs, 15, P1_COL);
    blobs++;
    
    BLOB_init(blobs, 15, P2_COL);
    blobs++;
    for(int i = 2; i < NUM_BLOBS; i++) {
        if ((i%2)==0){
        BLOB_init(blobs,8, P1_COL);
        }
        else{
        BLOB_init(blobs,8, P2_COL);
        }
        blobs++;
    }
}

int main (void) {
    
    int* p1_buttons;
    int* p2_buttons;
    
    float ax1, ay1, az1;
    float ax2, ay2, az2;
    
    
    // Ask the user to choose (via pushbuttons)
    // to play in single- or multi-player mode.
    // Use their choice to correctly initialize the game synchronizer.
    game_init();  
    
    // Keep an array of blobs. Use blob 0 for player 1 and
    // blob 1 for player 2.
    BLOB blobs[NUM_BLOBS];
        
    // Pass in a pointer to the blobs array. Iterate over the array
    // and initialize each blob with BLOB_init(). Set the radii and colors
    // anyway you see fit.
    generate_blobs(blobs);

    while(true) {
        //Lights up blue light when Player 1 leading, lights up green when tied and red when Player 2 leading. Light flashes when Score is close to win.
        if (score1>score2){
            if (score1>=7){
            myRGBLed.write(0,0,1);
            wait (0.1);
            myRGBLed.write(0,0,0);
            wait (0.1);
            }
            else{
            myRGBLed.write(0,0,1);  
            }
           }
        else if (score2>score1){
            if (score2>7){
            myRGBLed.write(1,0,0);
            wait (0.1);
            myRGBLed.write(0,0,0);
            wait (0.1);
            }
            else{
            myRGBLed.write(1,0,0);  
            }
           }
        else{
            myRGBLed.write(0,1,0);
            }
            
        
        
        
        // In single-player, check to see if the player has eaten all other blobs.
        // In multi-player mode, check to see if the players have tied by eating half the food each.
        // ***
        if(sync -> play_mode==SINGLE_PLAYER) {
            if (score1 == 10) 
            {game_over(WINNER_P1);}
            }
        else if (sync -> play_mode==MULTI_PLAYER) {
            if (score1 == 10 && score2 == 10) {game_over(WINNER_TIE);}
            }
        
        
        // In both single- and multi-player modes, display the score(s) in an appropriate manner.
        // ***
        uLCD.textbackground_color(BGRD_COL);
        char string1[100];
        char string2[100];
        char string3[100];
        sprintf(string1, "P1 Score: %d", score1);   //might go off screen use P1
        sprintf(string2, "P2 Score: %d", score2);
        sprintf(string3, "POWERS: %d", pwcount);
        
        GS_locate (sync, SCREEN_P1, 0, 0);
        GS_puts(sync, SCREEN_P1, string1, 15);
        GS_locate (sync, SCREEN_P1, 0, 1);
        GS_puts (sync, SCREEN_P1, string2, 15);
        GS_locate(sync, SCREEN_P1, 0, 2);
        GS_puts (sync, SCREEN_P1, string3, 15);
        
        if((sync -> play_mode==MULTI_PLAYER)) {
        GS_locate (sync, SCREEN_P2, 0, 0);
        GS_puts(sync, SCREEN_P2, string1, 12);
        GS_locate (sync, SCREEN_P2, 0, 1);
        GS_puts (sync, SCREEN_P2, string2, 12);
        GS_locate(sync, SCREEN_P1, 0, 2);
        GS_puts (sync, SCREEN_P1, string3, 15);
        }
        
        // Use the game synchronizer API to get the button values from both players' mbeds.
        p1_buttons = GS_get_p1_buttons(sync);
        p2_buttons = GS_get_p2_buttons(sync);
        
        // Use the game synchronizer API to get the accelerometer values from both players' mbeds.
        GS_get_p1_accel_data(sync, &ax1, &ay1, &az1);
        GS_get_p2_accel_data(sync, &ax2, &ay2, &az2);
        
        
        // If the magnitude of the p1 x and/or y accelerometer values exceed ACC_THRESHOLD,
        // set the blob 0 velocities to be proportional to the accelerometer values.
        // ***
        if (PLAYER1 == sync -> p1_p2) {
        if (abs(ax1) < ACC_THRESHOLD) {
            blobs[0].vx = 0;
            }
        else {
             blobs[0].vx = -ax1*(9000/blobs[0].rad);
             } //might change ax to positive 
                
        if (abs(ay1) < ACC_THRESHOLD) {
            blobs[0].vy = 0;
            }
        else {
            blobs[0].vy = ay1*(9000/blobs[0].rad);
            }
         }
                
                
        
        // If in multi-player mode and the magnitude of the p2 x and/or y accelerometer values exceed ACC_THRESHOLD,
        // set the blob 0 velocities to be proportional to the accelerometer values.
        // ***
        
        if ((sync -> play_mode==MULTI_PLAYER)&& (sync -> p1_p2 ==PLAYER2) ) {
            if (abs(ax2) < ACC_THRESHOLD) {
                blobs[1].vx = 0;
                }
            else {
                blobs[1].vx = -ax2*(9000/blobs[1].rad);
                } //might change ax to positive 
                
            if (abs(ay2) < ACC_THRESHOLD) {
                blobs[1].vy = 0;
                }
            else {
                blobs[1].vy = ay2*(9000/blobs[1].rad);
                }
             }
        float time_step = .01; 
        
        //POWER UP Function
       //Push button U for increasing velocity and R for increasing radius. I tried implementing time function but does not work.
    if (pwcount>0){   
        if(!pb_u) {
            t.start();
            blobs[0].vx=15*blobs[0].vx ;
            pwcount--;
        }
        
        if(!pb_r) {
            t.start();
            GS_circle(sync, SCREEN_P1, blobs[0].posx - blobs[0].posx, blobs[0].posy - blobs[0].posy, blobs[0].rad, BGRD_COL);
            blobs[0].rad=blobs[0].rad+5;          
            GS_circle(sync, SCREEN_P1, blobs[0].posx - blobs[0].posx, blobs[0].posy - blobs[0].posy, blobs[0].rad, blobs[0].color);
            pwcount--;    
        }   
    }   
    float tm=t.read();
        if  (tm==3){
            blobs[0].vx=(blobs[0].vx)/15 ;
            timcnt=50;
            t.reset();
        }
                
        if  (tm==3){
            //GS_circle(sync, SCREEN_P1, blobs[0].posx - blobs[0].posx, blobs[0].posy - blobs[0].posy, blobs[0].rad, BGRD_COL);
            blobs[0].rad= blobs[0].rad-5;
            //GS_circle(sync, SCREEN_P1, blobs[0].posx - blobs[0].posx, blobs[0].posy - blobs[0].posy, blobs[0].rad, blobs[i].color);
            t.reset();
        }
        pc.printf("%d\r\n",t.read());
        
        // Undraw the world bounding rectangle (use BGRD_COL).
        // ***
        
        GS_rectangle (sync, SCREEN_P1, (WORLD_WIDTH/-2) - blobs[0].old_x, (WORLD_HEIGHT/2) - blobs[0].old_y, (WORLD_WIDTH/2) - blobs[0].old_x, (WORLD_HEIGHT/-2) - blobs[0].old_y, BGRD_COL);
        
        if(MULTI_PLAYER == sync -> play_mode) {
            GS_rectangle (sync, SCREEN_P2, (WORLD_WIDTH/-2) - blobs[1].old_x, (WORLD_HEIGHT/2) - blobs[1].old_y, (WORLD_WIDTH/2) - blobs[1].old_x, (WORLD_HEIGHT/-2) - blobs[1].old_y, BGRD_COL);
    }
        // Loop over all blobs
        // ***
            int i;
            for (i = 0; i < NUM_BLOBS; i++) {
                
            // If the current blob is valid, or it was deleted in the last frame, (delete_now is true), then draw a background colored circle over
            // the old position of the blob. (If delete_now is true, reset delete_now to false.)  
            // ***
            if (blobs[i].valid || blobs[i].delete_now) {
                GS_circle(sync, SCREEN_P1, blobs[i].old_x - blobs[0].old_x, blobs[i].old_y - blobs[0].old_y, blobs[i].rad, BGRD_COL);
                }
            if (MULTI_PLAYER == sync -> play_mode) {
                GS_circle(sync, SCREEN_P2, blobs[i].old_x - blobs[1].old_x, blobs[i].old_y - blobs[1].old_y, blobs[i].rad, BLACK);
                }
            
            if (blobs[i].delete_now) {
                blobs[i].delete_now = false;
                }
                
            // Use the blob positions and velocities, as well as the time_step to compute the new position of the blob.
            // ***
           if (blobs[i].valid==true){ 
            blobs[i].posx = blobs[i].old_x + (blobs[i].vx) * time_step;
            blobs[i].posy = blobs[i].old_y + (blobs[i].vy) * time_step;
            }

            
            // If the current blob is blob 0, iterate over all blobs and check if they are close enough to eat or be eaten. 
            // In multi-player mode, if the player 0 blob is eaten, player 1 wins and vise versa.        
            // If blob 0 eats some food, increment score1.   
            // ***
            
            if (i == 0) {
                for(int j = 1; j < NUM_BLOBS; j++) {
                    float dist = BLOB_dist2(blobs[i], blobs[j]);
                    
                    
                    
                    if (j!=1){
                       if ((j%2)!=0){
                        if ((sqrt(dist) <= (blobs[i].rad+ blobs[j].rad))&& blobs[j].valid){
                           blobs[i].rad=  blobs[i].rad+ 1;
                           blobs[j].valid= false;
                           blobs[j].delete_now= true;
                           blobs[j].vx=0;
                           blobs[j].vy=0;
                           score1++;
                           playSound(bep);
                           
                        }
                       }
                       else {
                          if ((sqrt(dist) <= (blobs[i].rad+ blobs[j].rad))&& blobs[j].valid){
                           blobs[i].vx= -(blobs[i].vx);
                           blobs[j].vx= -(blobs[j].vx);
                           blobs[i].vy= -(blobs[i].vy);
                           blobs[j].vy= -(blobs[j].vy);
                        }
                        }
                    }   
                    else {
                        if ((sqrt(dist) <= (blobs[i].rad+ blobs[j].rad)&& blobs[1].valid)){                     
                            if ((blobs[i].rad)>(blobs[j].rad)){
                                
                                playSound(BUZZ);
                                game_over(WINNER_P1);
                                }
                            else if (blobs[0].rad < blobs[1].rad) {
                                playSound(BUZZ);
                                game_over(WINNER_P2);
                                
                            }
                            else if (blobs[0].rad == blobs[1].rad) {
                                blobs[i].vx= -(blobs[i].vx);
                                blobs[j].vx= -(blobs[j].vx);
                                blobs[i].vy= -(blobs[i].vy);
                                blobs[j].vy= -(blobs[j].vy);
                                }
                        }
                    }
                }
            }

                           
            
            // If the current blob is blob 1 and we are playing in multi-player mode, iterate over all blobs and check
            // if they are close enough to eat or be eaten. In multi-player mode, if the player 1 blob is eaten, player 0 wins and vise versa.
            // If blob1 eats some food, increment score 2.
            // ***
            
            if (i == 1) {
                for(int j = 0; j < NUM_BLOBS; j++) {
                    float dist = BLOB_dist2(blobs[1], blobs[j]);
                    
                    
                    if (j == 1) {
                        continue;
                        }
                    else if (j == 0) {
                        if ((sqrt(dist) <= (blobs[i].rad+ blobs[0].rad)&& blobs[0].valid)){                     
                            if ((blobs[i].rad)>(blobs[j].rad)){
                                playSound(BUZZ);
                                game_over(WINNER_P2);
                                
                                }
                            else if (blobs[0].rad > blobs[1].rad) {
                                playSound(BUZZ);
                                game_over(WINNER_P1);
                                
                                }
                            else {
                                blobs[i].vx= -(blobs[i].vx);
                                blobs[j].vx= -(blobs[j].vx);
                                blobs[i].vy= -(blobs[i].vy);
                                blobs[j].vy= -(blobs[j].vy);
                                }
                            }
                        }
                    
                    else {
                        if ((j%2)==0){
                        if ((sqrt(dist) <= (blobs[i].rad+ blobs[j].rad))&& blobs[j].valid){
                           blobs[i].rad=  blobs[i].rad+ 1;
                           blobs[j].valid= false;
                           blobs[j].delete_now= true;
                           score2++;
                           blobs[j].vx=0;
                           blobs[j].vy=0;
                           playSound(blip);
                            }
                        }
                       else {
                          if ((sqrt(dist) <= (blobs[i].rad+ blobs[j].rad))&& blobs[j].valid){
                           blobs[i].vx= -(blobs[i].vx);
                           blobs[j].vx= -(blobs[j].vx);
                           blobs[i].vy= -(blobs[i].vy);
                           blobs[j].vy= -(blobs[j].vy);
                            }
                        }   
                              
                    }
                 }
            }
            
            //AI for Player 2
            //AI for HARD, makes other blobs move towards P2, and P2 move away from P1 if radius less than P1 and vice versa.
            //AI for Medium makes other blobs move towards P2
            //AI for Easy makes everything move towards P1
            if ((sync->play_mode)==SINGLE_PLAYER){
                if (difficulty== HARD){
                    if (blobs[0].rad>blobs[1].rad){
                        blobs[1].vx= -(9000/blobs[1].rad)*(blobs[0].posx- blobs[1].posx);
                        blobs[1].vy= -(9000/blobs[1].rad)*(blobs[0].posy- blobs[1].posy);
                        
                        for (int p=2; p<22;p++){
                            float dist = sqrt(BLOB_dist2(blobs[1], blobs[p]));
                            if (dist<(2*((blobs[1].rad)+(blobs[p].rad)))){
                            blobs[1].vx= (9000/blobs[1].rad)*(blobs[p].posx- blobs[1].posx);
                            blobs[1].vy= (9000/blobs[1].rad)*(blobs[p].posy- blobs[1].posy);
                            blobs[p].vx= (500/blobs[p].rad)*(blobs[1].posx- blobs[p].posx);
                            blobs[p].vy= (500/blobs[p].rad)*(blobs[1].posy- blobs[p].posy);
                            }   
                            p=p+2;
                        }
                        }
                    else if (blobs[0].rad<blobs[1].rad){
                        blobs[1].vx= (9000/blobs[1].rad)*(blobs[0].posx- blobs[1].posx);
                        blobs[1].vy= (9000/blobs[1].rad)*(blobs[0].posy- blobs[1].posy);
                        }
                    else{
                        for (int p=2; p<22;p++){
                            float dist = sqrt(BLOB_dist2(blobs[1], blobs[p]));
                            /*if (dist<(2*((blobs[1].rad)+(blobs[p].rad)))){
                            blobs[1].vx= (9000/blobs[1].rad)*(blobs[p].posx- blobs[1].posx);
                            blobs[1].vy= (9000/blobs[1].rad)*(blobs[p].posy- blobs[1].posy);
                            } */
                            blobs[p].vx= (500/blobs[p].rad)*(blobs[1].posx- blobs[p].posx);
                            blobs[p].vy= (500/blobs[p].rad)*(blobs[1].posy- blobs[p].posy);
                            p=p+2;
                            }
                    blobs[1].vx=1.5*(blobs[1].vx);
                    blobs[1].vy=1.5*(blobs[1].vy);
                    blobs[0].vx=0.75*(blobs[0].vx);
                    blobs[0].vy=0.75*(blobs[0].vy);
                    
                    }
                }
                
            else if (difficulty==MEDIUM){
                for (int p=2; p<22;p++){
                           float dist = sqrt(BLOB_dist2(blobs[1], blobs[p]));
                            if (dist<(2*((blobs[1].rad)+(blobs[p].rad)))){
                            blobs[1].vx= (9000/blobs[1].rad)*(blobs[p].posx- blobs[1].posx);
                            blobs[1].vy= (9000/blobs[1].rad)*(blobs[p].posy- blobs[1].posy);
                            }
                            }
                }
            else if (difficulty==EASY){
                if (blobs[0].rad>blobs[1].rad){
                        blobs[1].vx= (9000/blobs[1].rad)*(blobs[0].posx- blobs[1].posx);
                        blobs[1].vy= (9000/blobs[1].rad)*(blobs[0].posy- blobs[1].posy);
                        }
                for (int p=3; p<22;p++){
                            blobs[p].vx= (100/blobs[p].rad)*(blobs[0].posx- blobs[p].posx);
                            blobs[p].vy= (100/blobs[p].rad)*(blobs[0].posy- blobs[p].posy);
                            p=p+2;
                            }
                }       
                        
            }
                
                
                            
                                        
            // You have to implement this function.
            // It should take in a pointer to a blob and constrain that blob to the world.
            // More details in blob.cpp
            BLOB_constrain2world(&blobs[i]);           
            
        }
        // Iterate over all blobs and draw them at their newly computed positions. Reference their positions to the player blobs.
        // That is, on screen 1, center the world on blob 0 and reference all blobs' position to that of blob 0.
        // On screen 2, center the world on blob 1 and reference all blobs' position tho that of blob 1.
        // ***
        for (i = 0; i < NUM_BLOBS; i++) {
         if (blobs[i].valid) {
                GS_circle(sync, SCREEN_P1, blobs[i].posx - blobs[0].posx, blobs[i].posy - blobs[0].posy, blobs[i].rad, blobs[i].color);
                }
            if (MULTI_PLAYER == sync -> play_mode) {
                GS_circle(sync, SCREEN_P2, blobs[i].posx - blobs[1].posx, blobs[i].posy - blobs[1].posy, blobs[i].rad, blobs[i].color);
                }
            
           blobs[i].old_x = blobs[i].posx;
           blobs[i].old_y = blobs[i].posy;
           
           }
           
        
        // Redraw the world boundary rectangle.
        // ***
        
        GS_rectangle (sync, SCREEN_P1, (WORLD_WIDTH/-2) - blobs[0].posx, (WORLD_HEIGHT/2) - blobs[0].posy, (WORLD_WIDTH/2) - blobs[0].posx, (WORLD_HEIGHT/-2) - blobs[0].posy, BORDER_COL);
        
        if(MULTI_PLAYER == sync -> play_mode) {
            GS_rectangle (sync, SCREEN_P2, (WORLD_WIDTH/-2) - blobs[1].posx, (WORLD_HEIGHT/2) - blobs[1].posy, (WORLD_WIDTH/2) - blobs[1].posx, (WORLD_HEIGHT/-2) - blobs[1].posy, BORDER_COL);
        }
        
        // Update the screens by calling GS_update.
        GS_update(sync);
        
        // If a button on either side is pressed, the corresponding LED on the player 1 mbed will toggle.
        // This is just for debug purposes, and to know that your button pushes are being registered.
        led1 = p1_buttons[0] ^ p2_buttons[0];
        led2 = p1_buttons[1] ^ p2_buttons[1];
        led3 = p1_buttons[2] ^ p2_buttons[2];
        led4 = p1_buttons[3] ^ p2_buttons[3];
    } 
    
}