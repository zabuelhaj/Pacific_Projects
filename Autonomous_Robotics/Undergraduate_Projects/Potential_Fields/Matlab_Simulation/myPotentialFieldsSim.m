function finalRad = myPotentialFieldsSim(robotObj)
    % Set constants for this program
    maxDuration= 1200;  % Max time to allow the program to run (s) 
    xNot = -2;
    yNot = -2;
    finish = 0;
    
    % Initialize loop variables
    tStart= tic;        % Time limit marker
    
    % Set initial robot location
    setMapStart(robotObj,[xNot yNot 0]);
    
    % Start robot driving forward
    SetFwdVelAngVel(robotObj,0.1,0);
    
    % Set local variables.
    xDest = 1;
    yDest = 2;
    yTan = yDest - yNot;
    xTan = xDest - xNot;
    
    % Set D. It can be anything within a range of D. The sensors are too
    % noisy tbh so I'm setting it to, like, 0.32 meters.
    D = 0.32;
    
    % First, turn to face the desired destination.
    dir = atan(yTan/xTan);
    TurnAngle(robotObj,0.2,dir);
    pause(0.1);
    
    % Enter main loop
    while ((toc(tStart) < maxDuration) && (autoCheck(robotObj)))
        % Read from the IR sensors.
        [dirF, magF] = repulsive(ReadIR(robotObj, 2), D);
        [dirL, magL] = repulsive(ReadIR(robotObj, 3), D);
        [dirR, magR] = repulsive(ReadIR(robotObj, 1), D);
        
        % Check to see if the robot has reached its destination.
        [x, y, th] = OverheadLocalization(robotObj);
        
        % Get the attractive vector.
        [dirG, magG] = move2goal(x, y, th, xDest, yDest);
        
        % Combine the vectors to get a direction of travel.
        dir = dirF + dirR + dirL + dirG;
        mag = magG - (magF + magL + magR);                       % These vectors are conflicting, so it only makes sense to subtract. Otherwise it goes out of range.
        
        % Travel in the desired direction.
        TurnAngle(robotObj, 0.2, dir);
        SetFwdVelAngVel(robotObj,mag*0.5,0);
        tL = tic;
        while (toc(tL) < 0.3)
        end
        
        % Check to see if the robot has reached its destination.
        [x, y, th] = OverheadLocalization(robotObj);
        if ((x <= (xDest + 0.1)) && (x >= xDest - 0.1)) && ((y <= (yDest + 0.1)) && (y >= yDest - 0.1))
            if (finish == 0)
                xDest = 1;
                yDest = -2;
                finish = 1;
            else
                break;
            end
        end

        % Briefly pause to avoid continuous loop iteration
        pause(0.1);
    end
    
end

function [dir, mag] = repulsive (d, D)
% This function will calculate the direction and magnitude that the robot
% will travel when faced with an object.

if (d <= D)
    dir = pi/2;
    mag = (D-d)/D;
else
    dir = 0;
    mag = 0;
end

end

function [dir, mag] = move2goal (x, y, th, xDest, yDest)
% This function will calculate the angle required to turn and the magnitude
% that the robot will travel. This represents the attraction to the goal
% for the robot.

% Calculate the difference of current pose and destination, then get
% required angle.
yTan = yDest - y;
xTan = xDest - x;

% First, turn to face the desired destination.
if (xTan == 0)
    dir = 0;
else
    dir = atan(yTan/xTan) - th;
end

% Second, set the magnitude. Because the robot is absolutely attracted to
% the goal, the magnitude will be max (so 1).
mag = 1;

end

function w = v2w(v)
% Calculate the maximum allowable angular velocity from the linear velocity
%
% Input:
% v - Forward velocity of Create (m/s)
%
% Output:
% w - Angular velocity of Create (rad/s)
    % Robot constants
    maxWheelVel= 0.5;   % Max linear velocity of each drive wheel (m/s)
    robotRadius= 0.08;   % Radius of the robot (m)

    % Max velocity combinations obey rule v+wr <= v_max
    w= (maxWheelVel-v)/robotRadius;
end