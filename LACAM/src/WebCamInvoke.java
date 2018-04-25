
import java.awt.Canvas;
import java.awt.Color;
import java.awt.HeadlessException;
import java.awt.Image;
import java.awt.Toolkit;
import java.util.List;
import javax.swing.JFrame;
import javax.swing.JPanel;
import sun.awt.windows.WComponentPeer;
import sun.awt.windows.WWindowPeer;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author umprasad
 */
public class WebCamInvoke extends JFrame{
    final static WebCam webCam=new WebCam();
    public WebCamInvoke() throws HeadlessException {
          final CaptuerPanel panel=new CaptuerPanel();
          panel.setVisible(true);
          getContentPane().add(panel);
          this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                    
     }
     public static void main(String args[]){
         try{
                WebCamInvoke invoke=new WebCamInvoke();
                invoke.setSize(1293,800);
                invoke.show(true);
         }catch(Exception ex){
             System.err.println(ex);
         }
    }
    
}
