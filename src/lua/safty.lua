print(project.name)
print(project.path)


if project.configurations.level == nil then
	print("No level")
else
	print("Level: " .. project.configurations.level)
end