# models.py
# Matias Blake, mb2522
# 12/04/2016
"""Models module for Breakout

This module contains the model classes for the Breakout game. That is anything that you
interact with on the screen is model: the paddle, the ball, and any of the bricks.

Technically, just because something is a model does not mean there has to be a special 
class for it.  Unless you need something special, both paddle and individual bricks could
just be instances of GRectangle.  However, we do need something special: collision 
detection.  That is why we have custom classes.

You are free to add new models to this module.  You may wish to do this when you add
new features to your game.  If you are unsure about whether to make a new class or 
not, please ask on Piazza."""
import random # To randomly generate the ball velocity
from constants import *
from game2d import *
wallSounds = [Sound('saucer1.wav'),Sound('saucer2.wav')]

# PRIMARY RULE: Models are not allowed to access anything except the module constants.py.
# If you need extra information from Play, then it should be a parameter in your method, 
# and Play should pass it as a argument when it calls the method.


class Paddle(GRectangle):
    """An instance is the game paddle.
    
    This class contains a method to detect collision with the ball, as well as move it
    left and right.  You may wish to add more features to this class.
    
    The attributes of this class are those inherited from GRectangle.
    
    LIST MORE ATTRIBUTES (AND THEIR INVARIANTS) HERE IF NECESSARY
    """
    # GETTERS AND SETTERS (ONLY ADD IF YOU NEED THEM)
    
    # INITIALIZER TO CREATE A NEW PADDLE
    
    # METHODS TO MOVE THE PADDLE AND CHECK FOR COLLISIONS
    def collides(self,ball):
        """Returns: True if the ball collides with the paddle
            
        Parameter ball: The ball to check
        Precondition: ball is of class Ball"""
        
        r=BALL_DIAMETER/2
        p1x=ball.x-r
        p1y=ball.y+r
        
        p2x=ball.x+r
        p2y=ball.y+r
        
        p3x=ball.x+r
        p3y=ball.y-r
        
        p4x=ball.x-r
        p4y=ball.y-r
        
        return (self.contains(p1x,p1y) or self.contains(p2x,p2y) or
        self.contains(p3x,p3y) or self.contains(p4x,p4y))
    
    
    # ADD MORE METHODS (PROPERLY SPECIFIED) AS NECESSARY


class Brick(GRectangle):
    """An instance is the game paddle.
    
    This class contains a method to detect collision with the ball.  You may wish to 
    add more features to this class.
    
    The attributes of this class are those inherited from GRectangle.
    
    LIST MORE ATTRIBUTES (AND THEIR INVARIANTS) HERE IF NECESSARY
    """
    
    # GETTERS AND SETTERS (ONLY ADD IF YOU NEED THEM)
    
    # INITIALIZER TO CREATE A BRICK

    # METHOD TO CHECK FOR COLLISION
    def collides(self,ball):
        """Returns: True if the ball collides with this brick
            
        Parameter ball: The ball to check
        Precondition: ball is of class Ball"""
        r=BALL_DIAMETER/2
        p1x=ball.x-r
        p1y=ball.y+r
        
        p2x=ball.x+r
        p2y=ball.y+r
        
        p3x=ball.x+r
        p3y=ball.y-r
        
        p4x=ball.x-r
        p4y=ball.y-r
        
        return (self.contains(p1x,p1y) or self.contains(p2x,p2y)
                or self.contains(p3x,p3y) or self.contains(p4x,p4y))
    # ADD MORE METHODS (PROPERLY SPECIFIED) AS NECESSARY


class Ball(GEllipse):
    """Instance is a game ball.
    
    We extend GEllipse because a ball must have additional attributes for velocity.
    This class adds this attributes and manages them.
    
    INSTANCE ATTRIBUTES:
        _vx [int or float]: Velocity in x direction 
        _vy [int or float]: Velocity in y direction 
    
    The class Play will need to look at these attributes, so you will need
    getters for them.  However, it is possible to write this assignment with no
    setters for the velocities.
    
    How? The only time the ball can change velocities is if it hits an obstacle
    (paddle or brick) or if it hits a wall.  Why not just write methods for these
    instead of using setters?  This cuts down on the amount of code in Gameplay.
    
    NOTE: The ball does not have to be a GEllipse. It could be an instance
    of GImage (why?). This change is allowed, but you must modify the class
    header up above.
    
    LIST MORE ATTRIBUTES (AND THEIR INVARIANTS) HERE IF NECESSARY
        
    """
    
    # GETTERS AND SETTERS (ONLY ADD IF YOU NEED THEM)
    def getVy(self):
        """Returns the vertical velocity of Ball object"""
        return self._vy
    
    def getY(self):
        """Returns the vertical coordinate of Ball object"""
        return self.y
    
    def setY(self,py):
        """Modifies the y coordinate of Ball object
           Parameter py: the new y coordinate
           Precondition: py is a float or int
        """
        self.y=py
    
    def setVy(self,m):
        """Sets the new _vy to the original (4.0) times m
           Parameter m: the number to multiply 4.0 with
           Precondition: m is of type float"""
        self._vy=4.0*m
    
    # INITIALIZER TO SET RANDOM VELOCITY
    def __init__(self,**keywords):
        """**Constructor**: Creates a new solid Ball
        
            :param keywords: dictionary of keyword arguments 
            **Precondition**:This class supports the all same keywords
            as `GEllipse`."""
            
        self._vy=-4.0
        self._vx = random.uniform(1.0,3.0) 
        self._vx = self._vx * random.choice([-1, 1])
        GRectangle.__init__(self,**keywords)
    # METHODS TO MOVE AND/OR BOUNCE THE BALL
    def update(self):
        """This method moves the ball by adding its x and y velocities
        to its x and y coordinates. If the edge of the ball touches the edge of
        the window, it calls bounce()"""
        #MOVE BALL
        self.x+=self._vx
        self.y+=self._vy
        ##
        
        #BOUNCE BALL
        r=BALL_DIAMETER/2
        if round(self.x-r)<=0 or round(self.x+r)>=GAME_WIDTH:
            #raise AssertionError('x')
            self.bounce('x')
        
        if self.y+r>=GAME_HEIGHT:
            self.bounce('y')
            #raise AssertionError('y')
 
    def bounce(self, d):
        """This method inverts either the x or the y velocity of the ball
           Parameter d: the velocity to invert
           Precondition: d is str and one of 'x' or 'y'
        """
        assert d=='x' or d=='y'
        
        if d=='x':
            random.choice(wallSounds).play()
            #print 'X COLLISION'
            self._vx=self._vx*-1
        elif d=='y':
            self._vy=self._vy*-1
            #print 'Y COLLISION'
    # ADD MORE METHODS (PROPERLY SPECIFIED) AS NECESSARY
    
    def Deadball(self):
        """Returns True if ball goes past lower boundary; False otherwise"""
        r=BALL_DIAMETER/2
        if self.y-r<=0:
            self.y=-1*r
            return True
        else:
            return False


# IF YOU NEED ADDITIONAL MODEL CLASSES, THEY GO HERE