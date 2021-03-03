import sys, os, shutil
from glob import glob

clean = '--clean' in sys.argv
fix = '--fix' in sys.argv

def fix_files(files, check_extension=True):
	for file_path in files:
		if 'vendor' in file_path:
			continue
		
		if not check_extension or file_path.endswith('.cpp') or file_path.endswith('.h') or file_path.endswith('.glsl'):
			print(f"Fixing {file_path}")
			rs = open(file_path)
			good = rs.read()
			if not good.endswith('\n'):
				good += '\n'
			rs.close()
			ws = open(file_path, 'wb')
			ws.write(bytearray(good, "utf8"))
			ws.close()

if fix:
	fix_files(glob("OverEngine/src/**", recursive=True))
	fix_files(glob("OverEngine/res/**", recursive=True))
	fix_files(glob("OverEditor/src/**", recursive=True))
	fix_files(glob("OverEditor/assets/**", recursive=True))
	fix_files(glob("Sandbox/src/**", recursive=True))
	fix_files(glob("Sandbox/assets/**", recursive=True))
	fix_files(glob("**/**.lua", recursive=True), False)
	fix_files(glob("**/imgui.ini", recursive=True), False)

if clean:

	to_remove = [
		"bin",
		"bin-int",
		"build",
		"build-linux",
		"OverEngine.sln",
	]

	for i in to_remove:
		if os.path.isdir(i):
			shutil.rmtree(i)
		elif os.path.isfile(i):
			os.remove("OverEngine.sln")


	for file_path in glob("OverEngine/vendor/*"):
		if (os.path.isdir(f"{file_path}/bin")):
			shutil.rmtree(f"{file_path}/bin")

		if (os.path.isdir(f"{file_path}/bin-int")):
			shutil.rmtree(f"{file_path}/bin-int")

		for file_path in glob(f"{file_path}/*"):
			if "vcxproj" in file_path:
				os.remove(file_path)

	for file_path in glob("*"):
		for file_path in glob(f"{file_path}/*"):
			wanna_delete = "vcxproj" in file_path
			wanna_delete = wanna_delete or file_path.endswith("OverEngine.log")
			wanna_delete = wanna_delete or file_path.endswith("OverEngineProfile-StartUp.json")
			wanna_delete = wanna_delete or file_path.endswith("OverEngineProfile-Runtime.json")
			wanna_delete = wanna_delete or file_path.endswith("OverEngineProfile-Shutdown.json")
			if wanna_delete:
				os.remove(file_path)
