
package student;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import models.Edge;
import models.Node;

/** This class contains Dijkstra's shortest-path algorithm and some other methods. */
public class Paths {
	
    /** Return the shortest valid path from start to end, or the empty list if
     * a path does not exist. A path is only valid if lives>0 after reaching
     * end.
     * Note: The empty list is NOT "null"; it is a list with 0 elements. */
    public static LinkedList<Node> shortestPath(Node start, Node end) {
        /* TODO Read note A7 FAQs on the course piazza for ALL details. */
        Heap<Node> F= new Heap<Node>(); // As in lecture slides

        // map contains an entry for each node in S or F. Thus,
        // |map| = |S| + |F|.
        // For each such key-node, the value part contains the shortest known
        // distance to the node and the node's backpointer on that shortest path.
        HashMap<Node, SFdata> map= new HashMap<Node, SFdata>();
        int lives = 3;//initial number of lives
        F.add(start, 0);
        map.put(start, new SFdata(0, null));
        // invariant: as in lecture slides, together with def of F and map
        while (F.size() != 0) {
            Node f= F.poll();
            if (f == end) return constructPath(end, map);
            double fDist= map.get(f).weight;
            
            for (Edge e : f.getExits()) {// for each neighbor w of f
                Node w= e.getOther(f);
                double newWdist=  fDist + weight(w,end,lives);
                SFdata wData= map.get(w);

                if (wData == null) { //if w not in S or F
                    map.put(w, new SFdata(newWdist, f));
                    F.add(w, newWdist);
                } else if (newWdist < wData.weight) {
                	if (w.isHostile()) lives-=1;
                    wData.weight= newWdist;
                    wData.backPointer= f;
                    F.updatePriority(w, newWdist);
                }
            }
        }

        // no path from start to end
        return new LinkedList<Node>();
    }

    /**return this Node's weight.
     * weight = distance from Node n to e) * change in speed
     * change in speed = 1.2 if hasSpeedUpgrade else if isHostile = 0.8
     *  else = 1
     *  if lives = 1 and n is hostile: p = practical infinity*/
    private static double weight(Node n,Node e, int l){
    	
    	double d = distance(n, e); //distance from n to Earth
    	double dv = 1;
    	if (n.hasSpeedUpgrade())dv+=10;
    	if (n.isHostile()){
    		dv-=0.2;
    		if (l == 1){
    			return d*10000000000.0;
    		}
    		
    	}
    	
    	return d*(dv);
    }
    
	/**return linear distance between two nodes*/
	private static double distance(Node a, Node b){
		
		double x = a.getX() - b.getX();
		double y = a.getY() - b.getY();
		
		return Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
		
	}
    
    
    
    /** Return the path from the start node to node end.
     *  Precondition: nData contains all the necessary information about
     *  the path. */
    public static LinkedList<Node> constructPath(Node end, HashMap<Node, SFdata> nData) {
        LinkedList<Node> path= new LinkedList<Node>();
        Node p= end;
        // invariant: All the nodes from p's successor to the end are in
        //            path, in reverse order.
        while (p != null) {
            path.addFirst(p);
            p= nData.get(p).backPointer;
        }
        return path;
    }

    /** Return the sum of the weights of the edges on path path. */
    public static int pathDistance(List<Node> path) {
        if (path.size() == 0) return 0;
        synchronized(path) {
            Iterator<Node> iter= path.iterator();
            Node p= iter.next();  // First node on path
            int s= 0;
            // invariant: s = sum of weights of edges from start to p
            while (iter.hasNext()) {
                Node q= iter.next();
                s= s + p.getConnect(q).length;
                p= q;
            }
            return s;
        }
    }
    

    
    
    /** An instance contains information about a node: the previous node
     *  on a shortest path from the start node to this node and the weight
     *  of this node from the start node. */
    private static class SFdata {
        private Node backPointer; // backpointer on path from start node to this one
        private double weight; // weight from start node to this one

        /** Constructor: an instance with distance d from the start node and
         *  backpointer p.*/
        private SFdata(double d, Node p) {
            weight= d;     // Weight from start node to this one.
            backPointer= p;  // Backpointer on the path (null if start node)
        }

        /** return a representation of this instance. */
        public String toString() {
            return "wgt " + weight + ", bckptr " + backPointer;
        }
    }
}
