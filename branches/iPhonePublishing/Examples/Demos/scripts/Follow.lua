-----------------------------------------------------------------------------------
-- Maratis
-- Follow script test
-----------------------------------------------------------------------------------

-- get objects
Obj1 = getObject("Obj1")
Obj2 = getObject("Obj2")

-- switch
switch = 1


-- get 2d vector length
function getLength2d(vec)

	return math.sqrt(vec[1]*vec[1] + vec[2]*vec[2])

end

-- normalize 2d vector
function normalize2d(vec)

	length = getLength2d(vec)
	vec[1] = vec[1] / length
	vec[2] = vec[2] / length
	return vec

end

-- lookAt
function lookAt(object, dir, rotSpeed)

	-- compute object X dir and Y dir
	rot = getRotation(object)

	zAngle = math.rad(rot[3])
	sinZ = math.sin(zAngle)
	cosZ = math.cos(zAngle)

	YDir = normalize2d({sinZ, -cosZ})
	XDir = {-YDir[2], YDir[1]}

	-- dot products for orientation and angle
	ori = (dir[1]*XDir[1] + dir[2]*XDir[2])
	angle = math.acos(dir[1]*YDir[1] + dir[2]*YDir[2])

	if ori < 0 then
		angle = - angle
	end

	rotate(object, {0, 0, 1}, math.deg(angle) * rotSpeed)

end


-- scene update
function onSceneUpdate()

	distanceMin = 15
	distanceMax = 100

	A = getPosition(Obj1)
	B = getPosition(Obj2)

	dir = {A[1] - B[1], A[2] - B[2]}

	-- A-B length
	length = getLength2d(dir)

	-- let's normalize dir (we have the length)
	dir[1] = dir[1] / length
	dir[2] = dir[2] / length

	-- switch 1
	if switch == 1 then

		if length > distanceMin then -- go near until distanceMin
			lookAt(Obj2, dir, 0.05)
			translate(Obj2, {0, -1, 0}, "local")
		else
			switch = 0 -- switch
		end

	-- switch 0
	else

		if length < distanceMax then -- go far until distanceMax
			-- inverse dir
			dir[1] = - dir[1]
			dir[2] = - dir[2]
			lookAt(Obj2, dir, 0.05)
			translate(Obj2, {0, -1, 0}, "local")
		else
			switch = 1 -- switch
		end

	end

end
