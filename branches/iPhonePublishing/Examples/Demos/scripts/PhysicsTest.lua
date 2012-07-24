-----------------------------------------------------------------------------------
-- Maratis
-- Physics script test
-----------------------------------------------------------------------------------

-- get objects
Car = getObject("Car")
W1 = getObject("W1")
W2 = getObject("W2")

power = 3
torque = 100

pos = getPosition(Car)

touch = 0

-- scene update
function onSceneUpdate()

	if getTouchPhase(0) == 0 then
		touch = 1
	elseif getTouchPhase(0) == 2 then
		touch = 0
	end
	
	x = 0
	force = 0
	newPos = getPosition(Car)
	dir = {newPos[1] - pos[1], newPos[2] - pos[2], newPos[3] - pos[3]}
	speed = math.sqrt(dir[1]*dir[1] + dir[2]*dir[2] + dir[3]*dir[3])

	-- control
	if isKeyPressed("UP") or touch == 1 then
		force = 1
	end

	if isKeyPressed("DOWN") then
		force = -1
	end

	if isKeyPressed("LEFT") and x==0 then
		x = -1
	end

	if isKeyPressed("RIGHT") and x==0 then
		x = 1
	end



	-- move
	if isCollisionTest(W1) or isCollisionTest(W2) then
		rot = speed*torque
		if rot > (torque/2) then
			rot = (torque/2)
		end
		addCentralForce(Car, {0, force*power, -power}, "local")
		addTorque(Car, {0, 0, -x*rot})
	end

	pos = newPos

end