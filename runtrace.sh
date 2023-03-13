cd build
make
rm meshViewer.trace
apitrace trace ./meshViewer
qapitrace meshViewer.trace
cd ..