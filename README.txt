/--------------------------------/
Edge.exe
Silhouette and Crease detection via image processing.

This program uses FBO's and multiple render targets
to output the color(original rendering), depth, and normal
values of the scene to three offscreen textures. The original
image, depth, and normal are all passed as textures to another
shader which detects the edges and composites the original
image with the detected edges.

Known Issues:
  The image processing checks neighboring pixels. To prevent excessive
  checks the texture coords simply wrap around at edges. This results in
  invalid edges being detected along texture borders. This can be fixed
  by checking over/underflow of UV coords. Performance impact unknown.


Author: Ian Stewart
	ikstewa@gmail.com
Date:	Nov 16, 2008

References:
	Decaudin, Philippe. Cartoon-Looking Rendering of 3D-Scenes. 1996

/--------------------------------/
Controls:

  'ESC' - Exit

  -- Depth image processing --
  'z' - enable/disable depth processing
  'a' - decrease edge threshold
  'q' - increase edge threshold

  -- Normal image processing --
  'x' - enable/disable normal processing
  's' - decrease edge threshold
  'w' - increase edge threshold

