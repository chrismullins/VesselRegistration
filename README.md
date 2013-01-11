Landmark Registration 
=====================

Overview
--------


### UNC Tumor Microenvironment Study
  *Data and publications from our collaboration with UNC on the quantification of vascular changes associated with tumors.
  *Project features acoustic angiography for imaging the tumor microenvironments and [TubeTK](http://www.tubetk.org) for quantifying those images.

I made this project in order to enable the fusion of ultrasound images of tumors in rodents.  We were provided images of two groups of animals (Day7 and Day5), each with separate US modalities (Brightness-Mode and Dual-Frequency).  For each of these subdirectories, there was a left image and a right image.  These needed to be fused by [TubeTK plugins](http://www.tubetk.org) to create the full image.

Fusing the images as they were resulted in the images being overlaid incorrectly, seen by shadowing effects in our initial attempts.  In order for TubeTK's optimization algorithm to find the global energy minimum of the vessel registration, it needed to be placed in the correct "ballpark," as many of the right images introduced rotations and small deformations.  

Using the landmarks obtained during the initial imaging study provided by Ryan Gessner, I employed [ITK's Landmark Based Transform Initializer](http://www.itk.org/Doxygen/html/classitk_1_1LandmarkBasedTransformInitializer.html) to find and write out the Versor Rigid 3D Transform which would map the coordinates of the right image onto the coordinates of the left.  

From here I applied the transform to the image in Slicer, and hardened and saved the transformed image.  Then I used the TubeTK plugin to merge the original left image with the now transformed right image.  


My Workflow
-----------
1. Clone and build the repository.

  <pre>
  $ git clone git://github.com:chrismullins/VesselRegistration.git
  $ mkdir VesselRegistration-build && cd  VesselRegistration-build
  $ cmake -DITK_DIR:PATH=/path/to/ITK-build ../VesselRegistration
  $ make
  </pre>

2. [Gather the data from MIDAS](midas3.kitware.com)

3. Gather the landmark data.

4. Transform the right images.  Let's assume the following:
  * Your landmarks are in a directory titled `~/TumorData/Landmarks`.
  * Your first images of interest are in a directory titled `~/TumorData/UpsampledMHDs_500group/DF`
A likely name for the first images of interest are `Animal508_df_left.mhd` and `Animal508_df_right.mhd` with their corresponding `.raw` files.
We will be using images in the [MetaIO format](http://www.itk.org/Wiki/ITK/MetaIO/Documentation#ITK_MetaIO).

  Run the transform:
 <pre>
 $ ./VesselRegistration \
 fixedImage.mhd movingImage.mhd \
 fixedLandmark.lnd movingLandmark.lnd \
 outputImageName.mhd outputTransformName.tfm
 </pre>

  In this case, it would translate to the following:
  <pre>
  $ ./VesselRegistration \
  ~/TumorData/UpsampledMHDs_500group/DF/Animal508_df_left.mhd ~/TumorData/UpsampledMHDs_500group/DF/Animal508_df_right.mhd \
  ~/TumorData/Landmarks/508_landmark_left.lnd ~/TumorData/Landmkarks/508_landmark_right.lnd \
  ~/TumorData/TransformedImages/Animal508_df_right_transformed.mhd ~/TumorData/Transforms/508_df_transform.tfm
  </pre>

5.  Now we should have an output image saved and a transform written out which maps the right image onto the left.  We could simply use the image that has been written out, but I've found that sometimes Slicer doesn't appreciate it.  So we can simply open up the original moving (right) image in Slicer, and apply the transform we've written using the [Slicer Transforms module](http://www.slicer.org/slicerWiki/index.php/Documentation/4.0/Modules/Transforms).

If you keep both the left and right images in the 3D viewer, you should be able to notice a difference.  Below is shown in Slicer some sample data before the transform is applied.  The axial view of the left (stationary) image is shown against the sagittal view of the right (moving) image, and they clearly do not line up.  

<img width="70%" src="http://www.slicer.org/slicerWiki/images/4/44/Unaligned_tumor_image.png"/>

A merge of these images "as is" would look like the top image:

<img width="70%" src="http://www.slicer.org/slicerWiki/images/4/4c/706_misregistration.jpg"/>

When you open Slicer, choose the "Add Data" icon, and load the original left and right images, and the transform file. Next, use the Modules searchbar to navigate to the "Transforms" module.  Choose the transform you loaded in the "Active Transform: " bar, and select the right (moving) image under the "Transformable" window at the bottom left.  Pressing the green arrow pointing to the right should apply the transform to the selected volume.  For reference, the transforms module sidebar should look similar to this:

<img width="50%" src="http://www.slicer.org/slicerWiki/images/b/b6/Transforms_module_for_tumor_merges.png"/>

Now you should notice that the images match more as you slice through the axial and sagittal views.  Here are some screenshots from my experience for reference - first is the two volumes viewed from the axial direction.  Notice how they match up from left to right.

<img width="70%" src="http://www.slicer.org/slicerWiki/images/6/62/Aligned_tumor_image_Axial-Axial.png"/>

Next is a screenshot of the same volumes, showing the right volume's sagittal view.  Notice the lineup from front to back.

<img width="70%" src="http://www.slicer.org/slicerWiki/images/e/ea/Aligned_tumor_image-Axial_Sagittal.png"/>

6.  Harden the transformed right volume in Slicer, and save it out.  This can be done by navigating to the "Data" module in the Modules toolbar.  Under "Nodes" you should see the transform, and the volume you transformed under it.  Right-click on the transformed volume in the sidebar and choose "Harden Transform" from the options presented.  Then by clicking the "Save" icon at the top you shoul be able to save the transformed image out to another file if you wish.

7.  Use the TubeTK extension to perform the merge.  

If you haven't already done so, download the TubeTK extension through the extension manager.  Click "View" and "Extension Manager" from the top toolbar.  If you're using a stable download of Slicer you should see lots of extensions available for download.  Choose TubeTK to install, and restart Slicer (make sure you've saved out your tranformed right image, if you have to restart Slicer!). 

If you haven't been using a stable Slicer package, you'll have to build your own extensions index following [these instructions](https://github.com/Slicer/ExtensionsIndex).  Good luck.

Now, navigate to the module named "Merge (TubeTK)" in the Modules searchbar.  If you've properly transformed the right image, the default settings should work, just choosing the left image as "Input Volume 1" and the right image as "Input Volume 2".  When you run this, it will take a while.
Checkout the [Slicer GUI TubeTK merge video](http://www.youtube.com/watch?v=RrypHDO3U4Y&feature=youtu.be) courtesy of Nathan Taylor.

Another option to apply the merge is to run TubeTK's tubeMerge plugin from the command line.  In order to do this, you'll have to [build TubeTK](http://public.kitware.com/Wiki/TubeTK/Development/Build).
  ```
  $ /path/to/TubeTk-build/bin/tubeMerge \
   ~/TumorData/UpsampledMHDs_500group/DF/Animal508_df_left.mhd -m \
   ~/TumorData/Output/TransformedInSlicer/Animal508_df_right_TransformedInSlicer.mhd \
   ~/TumorData/Output/Merged/Animal508_df_MERGED.mhd
  ```
### Using the Slicer Python interface
Alternatively, you may wish to run several merges as a batch process while you are away.  This can be done fairly easily with a python script.
I've defined a function which you can copy and paste into the python interactor which will invoke the tubeMerge process on two arguments (stationary and moving volumes).
  ```python
  def tubeMerge(volumeNodeStationary, volumeNodeMoving):
   parameters = {}
   parameters["inputVolume1"] = volumeNodeStationary.GetID()
   parameters["inputVolume2"] = volumeNodeMoving.GetID()
   outModel = slicer.vtkMRMLModelNode()
   slicer.mrmlScene.AddNode( outModel )
   parameters["outputVolume"] = outModel.GetID()
   tubeMergeCLI = slicer.modules.tubemerge
   return (slicer.cli.run(tubeMergeCLI, None, parameters))
  ```
After you've defined this method, go ahead and load the volumes you want to merge.
  ```python
  >>> slicer.util.loadVolume("/home/crmullin/TumorData/UpsampledMHDs_500group/DF/Animal508_df_left.mhd")
  >>> slicer.util.loadVolume("/home/crmullin/TumorData/Output/TransformedInSlicer/Animal508_df_right_TransformedInSlicer.mhd")
  >>> l = getNode('Animal508_df_left')
  >>> r = getNode('Animal508_df_right')
  >>> tm = tubeMerge(l, r)
  ```
Note that the `tubeMerge` module will run in a background thread, so the call to `tubeMerge()` will return immediately.  For help writing a script to do this repeatedly, check out the documentation on [running a CLI from Python in Slicer](http://wiki.slicer.org/slicerWiki/index.php/Slicer4:Python#Running_a_CLI_from_Python).

This should perform the merge after an hour or so (for a large volume).  TubeTK will output a compressed file ending in `.zraw`.
Decompress it by using the `ImageMath` executable built into TubeTK.

  ```
  $ /path/to/TubeTk-build/bin/ImageMath \
  ~/TumorData/Output/Merged/Animal508_df_MERGED.mhd -W 3 \
  ~/TumorData/Output/Merged/Decompressed/Animal508_df_MERGED_decompressed.mhd
  ```

If the merge is successful, you should be able to open Slicer (or another tool) and view an image that looks something like this.  Notice the offset and the rotation:

<img width="%70" src="http://www.slicer.org/slicerWiki/images/7/7b/Tumor_final_merge_tubetk.png"/>






