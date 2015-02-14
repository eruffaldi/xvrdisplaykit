set VERSION=1.0
del displaypack-%VERSION%.zip
zip -r displaypack-%VERSION%.zip *.* -x *svn* -x *.*~ 
