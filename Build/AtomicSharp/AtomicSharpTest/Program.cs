
using System;
using System.Collections.Generic;
using AtomicEngine;
using AtomicPlayer;

class BunnyMark : ScriptObject
{
	class Bunny
	{
		public Node node;
		public Vector2 position = new Vector2 ();
		public Vector2 speed = new Vector2 ();
	}

	public BunnyMark ()
	{
		var player = Atomic.GetSubsystem<Player> ();

		player.LoadScene ("Scenes/Scene.scene");	
		scene = player.CurrentScene;

		SubscribeToEvent ("Update", handleUpdate);
		SubscribeToEvent ("MouseButtonDown", handleMouseButtonDown);
		SubscribeToEvent ("MouseButtonUp", handleMouseButtonUp);

		var graphics = Atomic.GetSubsystem<Graphics> ();
		var cache = Atomic.GetSubsystem<ResourceCache> ();

		halfWidth = graphics.Width * pixelSize * 0.5f;
		halfHeight = graphics.Height * pixelSize * 0.5f;

		maxX = halfWidth;
		minX = -halfWidth;
		maxY = halfHeight;
		minY = -halfHeight;

		// TODO: generic version of this
		var sheet = (SpriteSheet2D)cache.GetResource ("SpriteSheet2D", "Sprites/bunnys_sheet.xml");

		var bunny1 = sheet.GetSprite ("bunny1");
		var bunny2 = sheet.GetSprite ("bunny2");
		var bunny3 = sheet.GetSprite ("bunny3");
		var bunny4 = sheet.GetSprite ("bunny4");
		var bunny5 = sheet.GetSprite ("bunny5");

		bunnyTextures = new Sprite2D[] { bunny1, bunny2, bunny3, bunny4, bunny5 };

		bunnyType = 2;
		currentTexture = bunnyTextures [bunnyType];

	}

	void handleMouseButtonDown (VariantMap eventData)
	{
		isAdding = true;
	}

	void handleMouseButtonUp (VariantMap eventData)
	{
		isAdding = false;
		bunnyType++;
		bunnyType %= 5;
		currentTexture = bunnyTextures [bunnyType];

	}

	void handleUpdate (VariantMap eventData)
	{

		if (isAdding) {

			var scale = new Vector2 ();
			var initPos = new Vector2 (minX, maxY);


			for (var i = 0; i < amount; i++) {

				var bunny = new Bunny ();
				bunnies.Add (bunny);

				var node = scene.CreateChild ();
				bunny.node = node;

				var sprite = (StaticSprite2D)node.CreateComponent ("StaticSprite2D");
				sprite.BlendMode = BlendMode.BLEND_ALPHA;
				sprite.Sprite = currentTexture;

				node.Position2D = bunny.position = initPos;
				bunny.speed.x = (float)(random.NextDouble () * 10);
				bunny.speed.y = (float)(random.NextDouble () * 10) - 5;


				scale.x = scale.y = (0.5f + ((float)random.NextDouble ()) * 0.5f);
				node.Scale2D = scale;

				node.Rotation2D = (((float)random.NextDouble ()) - 0.5f);
			}
		}

		foreach (var bunny in bunnies) {

			var px = bunny.position.x;
			var py = bunny.position.y;

			var speedX = bunny.speed.x;
			var speedY = bunny.speed.y;

			px += speedX * .002f;
			py += speedY * .002f;

			if (px > maxX) {
				speedX *= -1;
				px = maxX;
			} else if (px < minX) {
				speedX *= -1;
				px = minX;
			}

			if (py > maxY) {
				speedY = 0;
				py = maxY;

			} else if (py < minY) {

				speedY *= -0.95f;

				if (((float)random.NextDouble ()) > 0.5f) {
					speedY -= ((float)random.NextDouble ()) * 6f;
				}

				py = minY;
			}

			bunny.speed.x = speedX;
			bunny.speed.y = speedY + gravity;

			bunny.position.x = px;
			bunny.position.y = py;
			bunny.node.Position2D = bunny.position;
			;

		}
				
	}

	Random random = new Random ();

	List<Bunny> bunnies = new List<Bunny> ();

	int amount = 3;
	float gravity = -0.5f;
	Scene scene;

	Sprite2D[] bunnyTextures;

	bool isAdding = false;

	int bunnyType = 2;
	Sprite2D currentTexture;


	float pixelSize = 0.01f;

	float halfWidth;
	float halfHeight;

	float maxX;
	float minX;
	float maxY;
	float minY;

}
	
enum MyEnum
{
	Peaceful,
	Friendly,
	Aggressive
}

class Spinner : CSComponent
{
	[Inspector]
	public int MyField = 42;

	[Inspector("Sprites/star.png")]
	public Sprite2D MySprite;

	[Inspector]
	public Vector3 Vector3Field = new Vector3(1, 2, 3);

	[Inspector]
	public MyEnum Attitude = MyEnum.Friendly;

	[Inspector]
	public Sprite2D[] SpritesField;

}

/*
class Spinner : CSComponent
{
	public float Speed = 1.0f;

	override public void Start()
	{
		myObject = new MyObject ();

		SubscribeToEvent (myObject, "MyEvent", handleMyObjectEvent);

		var renderer = Atomic.GetSubsystem<Renderer> ();
		SubscribeToEvent (renderer, "BeginViewUpdate", handleEvent);

		SubscribeToEvent (this, "SelfEvent", handleSelfEvent);

		SendEvent ("SelfEvent");
	}

	void handleSelfEvent(VariantMap eventData)
	{
		Console.WriteLine ("Got Self Event");		
	}
		
	void handleMyObjectEvent(VariantMap eventData)
	{
		//Console.WriteLine ("Got My Event");		
	}

	void handleEvent(VariantMap eventData)
	{
		View view = eventData.Get<View> ("view");
		view.Camera.Zoom = zoom;
		zoom += .01f;

		myObject.SendEvent ("MyEvent");
	}

	override public void Update(float timeStep)
	{
		Node.Yaw (timeStep * 75 * Speed);
	}

	float zoom = 1.0f;

	MyObject myObject;

}
*/
	
class MyGame
{
	public static void Main (string[] args)
	{		

		Atomic.RegisterAssemblyComponents (typeof(MyGame).Assembly);

		var bunnyMark = new BunnyMark ();

		Atomic.Run ();

	}
}


