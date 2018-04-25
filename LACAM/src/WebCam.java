
import java.awt.Component;
import java.awt.Image;
import java.awt.peer.ComponentPeer;
import java.awt.peer.WindowPeer;
import java.util.List;
import javax.swing.JPanel;
import sun.awt.windows.WComponentPeer;


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author umprasad
 */
public class WebCam{
   public static final String path="C:\\webcame\\webcame.dll"; 
   public native List<String> getDevices();
   public native int connect(String deviceName,Component component);
   public native void clipBoard(int hwnd);
   public native byte[] getImage(int hwnd); 
   public native void disConnect(int hwnd);
   //public native int createWindow(String title);
   static{
       try{
            
            System.load(path);
    
       }catch(Exception ex){
           System.err.println("WebCame{}"+ex.getMessage());
       }
   }
   public void loadPanel(final JPanel panel){
        List<String> devices=getDevices();
        connect(devices.get(0),panel);
        WComponentPeer peer=(WComponentPeer)panel.getPeer();
        peer.getHWnd();
   }
}
