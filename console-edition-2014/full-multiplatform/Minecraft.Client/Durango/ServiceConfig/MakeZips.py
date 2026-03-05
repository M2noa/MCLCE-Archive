
import os
import shutil
import zipfile

# 4J-JEV: Takes:
#	- Localisation from 'ServiceConfig\loc\ex-EX\index.html'
#	- Template help from 'ServiceConfig\HelpDocument\*'
#
# Then:
#	- Constructs all zipped files to 'ServiceConfig\HelpDocument_ex-EX.zip'.
#
# NOTE: Make sure to check out 'ServiceConfig\HelpDocument\*' first.

def formatLoc(str):
	[lang,local] = str.split('-')
	return ( lang.lower() + "-" + local.upper() )

def copyTemplate(dst):
	for root, dirs, files in os.walk(".\\HelpDocument\\"):
		if not os.path.exists(dst+"\\"+root):
			os.makedirs(dst+"\\"+root)
	
		for f in filter(lambda x: x!="index.html", files):
			if not os.path.isdir(root+"\\"+f):
				print "Copying to '%s\\%s\\%s'" % (dst,root,f)
				shutil.copyfile(root+"\\"+f, dst+"\\"+root+"\\"+f)

def createZip(name):
	os.chdir(".\\"+name)

	zipname = name+".zip"
	print "Created "+zipname
	
	z = zipfile.ZipFile(zipname, 'w')
	for root, dirs, files in os.walk(".\\HelpDocument\\"):
		for file in files:
			print "Adding '%s\\%s'." % (root,file)
			z.write(os.path.join(root,file))
			
	z.close()
	
	shutil.move(".\\"+zipname, "..\\"+zipname)
	os.chdir("..")

	
	
						#== MAIN ==#	
						
if __name__=="__main__":
	for loc in map(formatLoc,os.listdir(".\\loc\\")):
		tardir = ".\\HelpDocument_"+loc
		
		if os.path.isdir(tardir):
			shutil.rmtree(tardir)
		copyTemplate(tardir+"\\")
		
		print ( "Making '%s'" % tardir )
		shutil.copy(".\\loc\\"+loc+"\\index.html",tardir+"\\HelpDocument\\index.html")
		
		createZip(tardir)
		shutil.rmtree(tardir)