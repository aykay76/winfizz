System:init
{
	System:Cameras:Add("cam1", Vector(0.0, 0.0, 100.0), Vector(0.0, 0.0, -1.0), Vector(0.0, 1.0, 0.0), 45.0);
	System:Cameras:Add("cam2", Vector(0.0, 0.0, -2000.0), Vector(0.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0), 45.0);
	System:Materials:Add("shinyred", Colour(0.5, 0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0, 0.0), 0.0);
	System:Lights:Add("ambient", Colour(1.0, 1.0, 1.0, 1.0), Colour(1.0, 0.0, 0.0, 1.0), Colour(1.0, 0.0, 0.0, 1.0), 0.0, 5000.0, -5000.0, 1.0, 0.0, -1.0, 0.0, 0.9, 45.0);
	System:Lights:Add("spotred01", Colour(1.0, 0.0, 0.0, 1.0), Colour(1.0, 0.0, 0.0, 1.0), Colour(1.0, 0.0, 0.0, 1.0), -50.0, -10.0, -100.0, 1.0, 0.0, 0.0, -1.0, 0.1, 10.0);
	System:Lights:Add("spotgreen", Colour(0.0, 1.0, 0.0, 1.0), Colour(0.0, 1.0, 0.0, 1.0), Colour(0.0, 1.0, 0.0, 1.0), 50.0, 10.0, -100.0, 1.0, 0.0, 0.0, -1.0, 0.91, 10.0);
	System:Templates:Add("car", "cubecube.3ds");
	System:Entities:Add("acar", "car", 0.0, 0.0, -1500.0, -90.0, 0.0, -25.0);
	System:Entities:Add("acar2", "car", -500.0, 500.0, -5000.0, -90.0, 0.0, -25.0);
	System:Textures:Add("face", "C:\myphoto.jpg");
	System:Timers:Add("t250ms", 40, 0);
	System:Timers:Add("t1000ms", 1000, 1);
} 

t250ms:update 
{
	System:Entities["acar"]:ax = System:Entities["acar"]:ax + 2.0;
}

t1000ms:update
{
	System:Lights["spotred01"]:ambient = Colour(0.0, 0.0, 1.0);
	System:Lights["spotred01"]:diffuse = Colour(0.0, 0.0, 1.0);
	System:Lights["spotred01"]:specular = Colour(0.0, 0.0, 1.0);

	System:Lights["spotgreen"]:ambient = Colour(1.0, 0.0, 0.0);
	System:Lights["spotgreen"]:diffuse = Colour(1.0, 0.0, 0.0);
	System:Lights["spotgreen"]:specular = Colour(1.0, 0.0, 0.0);

	System:Timers:Add("lightson", 1000, 1);
}

lightson:update
{
	System:Lights["spotred01"]:ambient = Colour(1.0, 0.0, 0.0, 0.0);
	System:Lights["spotred01"]:diffuse = Colour(1.0, 0.0, 0.0, 0.0);
	System:Lights["spotred01"]:specular = Colour(1.0, 0.0, 0.0, 0.0);

	System:Lights["spotgreen"]:ambient = Colour(0.0, 1.0, 0.0, 0.0);
	System:Lights["spotgreen"]:diffuse = Colour(0.0, 1.0, 0.0, 0.0);
	System:Lights["spotgreen"]:specular = Colour(0.0, 1.0, 0.0, 0.0);

	System:camera = "cam1";
	System:Timers:Add("t1000ms", 1000, 1);
}

System:mouseMove
{
}
