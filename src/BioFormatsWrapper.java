import java.io.IOException;
import loci.common.services.ServiceFactory;
import loci.formats.ImageReader;
import loci.formats.meta.IMetadata;
import loci.formats.services.OMEXMLService;
import loci.formats.FormatException;

// example to read OME metadata and data matrix from image file
// https://github.com/ome/bioformats/blob/develop/components/formats-gpl/utils/MultiFileExample.java
// https://github.com/ome/bioformats/blob/develop/components/formats-gpl/utils/ParallelRead.java

public class BioFormatsWrapper {
    public String getMetadata(String filePath) {
        try {
            ImageReader reader = new ImageReader();
            ServiceFactory factory = new ServiceFactory();
            OMEXMLService service = factory.getInstance(OMEXMLService.class);
            IMetadata meta = service.createOMEXMLMetadata();
            reader.setMetadataStore(meta);
            reader.setId(filePath);
            System.out.println(Thread.currentThread().getName() +
              ": path=" + filePath +
              ", sizeX=" + reader.getSizeX() +
              ", sizeY=" + reader.getSizeY() +
              ", sizeZ=" + reader.getSizeZ() +
              ", sizeT=" + reader.getSizeT() +
              ", sizeC=" + reader.getSizeC() +
              ", imageName=" + meta.getImageName(0));
              reader.close();

            // return "Image Dimensions: " + sizeX + " x " + sizeY;

            System.out.println(filePath);
            return "Hello from Java Wrapper";
        } catch (Exception e) {
            e.printStackTrace();
            return "Error: " + e.getMessage();
        }
    }
}