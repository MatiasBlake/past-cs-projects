# play.py
# Matias Blake, mb2522
# 12/04/2016
"""Subcontroller module for Breakout

This module contains the subcontroller to manage a single game in the Breakout App. 
Instances of Play represent a single game.  If you want to restart a new game, you are 
expected to make a new instance of Play.

The subcontroller Play manages the paddle, ball, and bricks.  These are model objects.  
Their classes are defined in models.py.

Most of your work on this assignment will be in either this module or models.py.
Whether a helper method belongs in this module or models.py is often a complicated
issue.  If you do not know, ask on Piazza and we will answer."""
from constants import *
from game2d import *
from models import *
import random

bounceSound = Sound('saucer1.wav')
breakSounds = [Sound('plate1.wav'),Sound('plate2.wav')]
serveSound = Sound('cup1.wav')



# PRIMARY RULE: Play can only access attributes in models.py via getters/setters
# Play is NOT allowed to access anything in breakout.py (Subcontrollers are not
# permitted to access anything in their parent. To see why, take CS 3152)


class Play(object):
    """An instance controls a single game of breakout.
    
    This subcontroller has a reference to the ball, paddle, and bricks. It animates the 
    ball, removing any bricks as necessary.  When the game is won, it stops animating.  
    You should create a NEW instance of Play (in Breakout) if you want to make a new game.
    
    If you want to pause the game, tell this controller to draw, but do not update.  See 
    subcontrollers.py from Lecture 25 for an example.
    
    INSTANCE ATTRIBUTES:
        _paddle [Paddle]: the paddle to play with 
        _bricks [list of Brick]: the list of bricks still remaining 
        _ball   [Ball, or None if waiting for a serve]:  the ball to animate
        _tries  [int >= 0]: the number of tries left
        
    
    As you can see, all of these attributes are hidden.  You may find that you want to
    access an attribute in class Breakout. It is okay if you do, but you MAY NOT ACCESS 
    THE ATTRIBUTES DIRECTLY. You must use a getter and/or setter for any attribute that 
    you need to access in Breakout.  Only add the getters and setters that you need for
    Breakout.
    
    You may change any of the attributes above as you see fit. For example, you may want
    to add new objects on the screen (e.g power-ups).  If you make changes, please list
    the changes with the invariants.
                  
    LIST MORE ATTRIBUTES (AND THEIR INVARIANTS) HERE IF NECESSARY
    
        _score [int]: The player's score (each brick is worth 10 points)
        _speed_multiplier [float]: the incrementer for the speed. Starts at 1.0 and increases
                                   by .1 every impact with the paddle
    """
    
    
    # GETTERS AND SETTERS (ONLY ADD IF YOU NEED THEM)
    def getLives(self):
        """Returns the number of tries left"""
        return self._tries
   
    def getBricks(self):
        """Returns the number of bricks left"""
        return len(self._bricks)
    
    def getScore(self):
        """Returns the player's current score"""
        return self._score
        
    # INITIALIZER (standard form) TO CREATE PADDLES AND BRICKS
    def __init__(self):
        """**Constructor**: Creates a new Play instance"""
        
        self._speed_multiplier=1.0
        self._score=0
        self._bricks=create_bricks()
        self._paddle=Paddle(height=PADDLE_HEIGHT,width=PADDLE_WIDTH,
                            y=PADDLE_OFFSET,fillcolor=colormodel.BLUE,
                            x=GAME_WIDTH/2)
        self._ball=None
        self._tries=NUMBER_TURNS
        
    # UPDATE METHODS TO MOVE PADDLE, SERVE AND MOVE THE BALL
    def updatePaddle(self,inp):
        """This method enables the user to move the paddle by using the left
           right arrows"""
        s=0
        d=6
        
        if inp.is_key_down('left'):
            if self._paddle.left < d:
                s+=0
            else:
                s-=1
        elif inp.is_key_down('right'):
            
            if self._paddle.right+d > GAME_WIDTH:
                s+=0
            else:
                s+=1
              
        self._paddle.x+=d*s


    def serveBall(self):
        """This method 'serves' the ball by calling the constructor Ball()"""
        serveSound.play()
        self._ball=Ball(width=BALL_DIAMETER,height=BALL_DIAMETER,
                        fillcolor=colormodel.RED,x=GAME_WIDTH/2,
                        y=GAME_HEIGHT/2)

    def updateBall(self):
        """This method calls the method to move the ball in module models
           if the ball object is not None"""
        if not self._ball is None:
            self._ball.update()
            
# METHOD TO DRAW THE PADDLES, BALL, AND BRICKS
    def draw(self,view):
        """Method to draw paddle, bricks and ball.
           This method is called by the draw method in module breakout"""
        #Draw bricks    
        for b in self._bricks: 
            b.draw(view)
        
        #Draw paddle
        p=self._paddle
        p.draw(view)
        
        #Draw Ball
        
        ball=self._ball
        if not ball is None:
            ball.draw(view)
        
    # HELPER METHODS FOR PHYSICS AND COLLISION DETECTION
    def brick_collision(self):
        """Deals with a brick collision.
           This method removes the impacted brick from _bricks"""
        
        for b in self._bricks:
            if b.collides(self._ball):
                self._ball.bounce('y')
                i=self._bricks.index(b)
                self.addScore(self._bricks[i])
                random.choice(breakSounds).play()
                del self._bricks[i]
                
    def paddle_collision(self):
        """Method to check if ball collides with paddle. And to bounce ball
           and increase the speed.
           If the ball hits the paddle while moving upwards (invalid bounce)
           then the ball's y coordinate is set to the boundary of the window,
           causing Deadball() to return True"""
           
        if self._paddle.collides(self._ball) and self._ball.getVy()<0:
            self._ball.bounce('y')
            if self._ball.getVy()<8.0:
                self._speed_multiplier+=0.05
                self._ball.setVy(self._speed_multiplier)
                bounceSound.play()
        elif self._paddle.collides(self._ball) and self._ball.getVy()>0:
            self._ball.setY(0)
            
    # ADD ANY ADDITIONAL METHODS (FULLY SPECIFIED) HERE
    def Deadball(self):
        """Returns True if ball goes past lower boundary; False otherwise
           Method to check if ball went past lower boundary and to adjust
           attribute _tries accordingly.
           This method works by calling the method Deadball() within the
           class Ball"""
           
        if self._ball.Deadball():
            self._tries-=1
            self._speed_multiplier=1
            return True
        return False

    def addScore(self,brick):
        """Adds ten points to the player's score
           Parameter brick: The brick the ball collided against
           Precondition: brick is a brick object"""
           
        value=abs(int(brick.y*self._ball.getVy())/4)
        self._score+=10*value


def create_bricks():
        """Returns a list of Brick objects
           A helper function to create bricks"""
        numrows=0
        numbricks=0
        bricks=[]
        while numrows<BRICK_ROWS:
           
            if numrows==0:
                py=GAME_HEIGHT-BRICK_Y_OFFSET
            else:
                py-=BRICK_SEP_V+BRICK_HEIGHT
                
            while numbricks<BRICKS_IN_ROW:
                if numbricks==0:
                    pl=BRICK_SEP_H/2
                else:
                    pl+=BRICK_WIDTH+BRICK_SEP_H
                c=int(str(numrows)[-1])
                
                bricks.append(Brick(width=BRICK_WIDTH,height=BRICK_HEIGHT,
                                    left=pl,y=py,fillcolor=BRICK_COLORS[c],
                                    linecolor=BRICK_COLORS[c]))
                numbricks+=1
            numbricks=0
            numrows+=1
        
        return bricks

