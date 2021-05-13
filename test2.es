System:init
{
	System:Cameras:Add("cam1", Vector(0.0, 10.0, 100.0), Vector(0.0, 0.0, -1.0), Vector(0.0, 1.0, 0.0), 45.0);
	System:Lights:Add("spotgreen", Colour(1.0, 1.0, 0.9, 1.0), Colour(1.0, 1.0, 0.9, 1.0), Colour(1.0, 1.0, 0.9, 1.0), 50.0, 300.0, -50.0, 1.0, 0.0, -0.2, 0.8, 0.1, 90.0);
}

t40ms:update 
{
}

t1000ms:update
{
}

lightson:update
{
}

System:mouseMove
{
}
