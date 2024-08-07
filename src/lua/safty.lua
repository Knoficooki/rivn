greeting = "hello World of Lua!"

print(greeting)

function hello(argc, argv)
	for i=0,argc do
		print(argv)
	end
end

hello(10, "Hello")

player = {
	"name" = "John Doe",
	"age" = 42,
	"isMale" = true,
	"skills" = {
		"lua" = {
			"version" = "5.3.5"
		},
	}
	"Family" = {
		"parents" = {
			"name" = "John",
			"age" = 42
		},
		"children" = {
			"name" = "Jane",
			"age" = 7
		},
	}
	"Level" = 20
}
