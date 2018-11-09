package student;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import models.Edge;
import models.Node;
import models.NodeStatus;
import models.RescueStage;
import models.ReturnStage;
import models.Spaceship;



/** An instance implements the methods needed to complete the mission */
public class MySpaceship extends Spaceship {

	Node Earth; //Earth node, initiaized in return stage
	ReturnStage st; //ReturnStage state, initialized in return stage
	LinkedList<Long> visited = new LinkedList<Long>();/**list of nodes visited
	in rescue stage*/

	LinkedList<Node> returnRoute = new LinkedList<Node>();//path 1 to earth
	int lives = 3;//lives for path 1 testing

	LinkedList<Node> returnRoute2 = new LinkedList<Node>(); //path 2 to earth
	int lives2 = 3; //lives for path 2 testing

	/**
	 * Explore the galaxy, trying to find the missing spaceship that has crashed
	 * on Planet X in as little time as possible. Once you find the missing
	 * spaceship, you must return from the function in order to symbolize that
	 * you've rescued it. If you continue to move after finding the spaceship
	 * rather than returning, it will not count. If you return from this
	 * function while not on Planet X, it will count as a failure.
	 * 
	 * At every step, you only know your current planet's ID and the ID of all
	 * neighboring planets, as well as the ping from the missing spaceship.
	 * 
	 * In order to get information about the current state, use functions
	 * currentLocation(), neighbors(), and getPing() in RescueStage. You know
	 * you are standing on Planet X when foundSpaceship() is true.
	 * 
	 * Use function moveTo(long id) in RescueStage to move to a neighboring
	 * planet by its ID. Doing this will change state to reflect your new
	 * position.
	 */
	@Override
	public void rescue(RescueStage state) {
		// TODO : Find the missing spaceship


		while (!state.foundSpaceship()){
			dfs(state);
		}




	}

	/**Perform depth-first search and return once planet x is found.
	 * Nodes are visited in decreasing order of ping
	 * */
	public void dfs(RescueStage state){
		if (state.foundSpaceship()) return;
		long cl = state.currentLocation();
		visited.add(cl);


		for (NodeStatus ns : neighborsByPing(state)){
			if (!visited.contains(ns.getId())){
				if (state.foundSpaceship()) return;
				//				System.out.println(idList(neighborsByPing(state)));
				//				System.out.println(ns.getId());
				state.moveTo(ns.getId());
				dfs(state);
			}
			else if (noMoNeighbors(state)) state.moveTo(visited.get((visited.indexOf(cl)-1)));
		}	
	}


	/**Return the list of neighbors in the current state in descending order of ping*/
	public LinkedList<NodeStatus> neighborsByPing(RescueStage state){

		NodeStatus[] ordered = state.neighbors().toArray(new NodeStatus[0]);
		Arrays.sort(ordered);

		LinkedList<NodeStatus> ll = new LinkedList<NodeStatus>();

		for (NodeStatus ns : ordered){
			ll.add(ns);
		}


		return ll;
	}

	/**Return a list of the ids of the nodes provided*/
	public LinkedList<Long> idList(LinkedList<NodeStatus> l){

		LinkedList<Long> ll = new LinkedList<Long>();

		for (NodeStatus ns : l){
			ll.add(ns.getId());
		}


		return ll;
	}


	/**Return a list of the pings of the nodes provided*/
	public LinkedList<Double> pingList(LinkedList<NodeStatus> l){

		LinkedList<Double> ll = new LinkedList<Double>();

		for (NodeStatus ns : l){
			ll.add(ns.getPingToTarget());
		}


		return ll;
	}

	/**returns true iff the current planet has no unvisited neighbors*/
	public boolean noMoNeighbors(RescueStage state){

		LinkedList<Long> neighborIDs = new LinkedList<Long>();

		for (NodeStatus ns : state.neighbors()){
			neighborIDs.add(ns.getId());
		}

		neighborIDs.removeAll(visited);

		return neighborIDs.isEmpty();



	}

	/**
	 * Get back to Earth, avoiding hostile troops and searching for speed
	 * upgrades on the way. Traveling through 3 or more planets that are hostile
	 * will prevent you from ever returning to Earth.
	 *
	 * You now have access to the entire underlying graph, which can be accessed
	 * through ScramState. currentNode() and getEarth() will return Node objects
	 * of interest, and getNodes() will return a collection of all nodes in the
	 * graph.
	 *
	 * You may use state.grabSpeedUpgrade() to get a speed upgrade if there is
	 * one, and can check whether a planet is hostile using the isHostile
	 * function in the Node class.
	 *
	 * You must return from this function while on Earth. Returning from the
	 * wrong location will be considered a failed run.
	 *
	 * You will always be able to return to Earth without passing through three
	 * hostile planets. However, returning to Earth faster will result in a
	 * better score, so you should look for ways to optimize your return.
	 */
	@Override
	public void returnToEarth(ReturnStage state) {
		// TODO: Return to Earth

		Earth = state.getEarth();
		st = state;

		/*path is calculated using a modified Dijkstra's algorithm*/
		LinkedList<Node> path = Paths.shortestPath(st.currentNode(), Earth);
		path.removeFirst(); //to avoid trying to travel to Planet X

		/*calculated using local method*/
		recursivePath(st.currentNode());

		/*check best path*/
		if (timeTaken(path)<timeTaken(returnRoute) && timeTaken(path)!=-1){
			
			travel(path);
			return;
		}


		travel(returnRoute);
	}

	/**Creates a path to earth and stores it in returnRoute*/
	private void recursivePath(Node current){

		if (!returnRoute.isEmpty()){if (returnRoute.getLast() == Earth) return;}
		if (current.isHostile()) lives -=1;
		Node b = findMinPrior(neighbors(current));
		returnRoute.add(b);
		recursivePath(b);
	}

	/**returns Node with smallest priority in provided list*/
	private Node findMinPrior(LinkedList<Node> l){
		Node min = l.getFirst();
		for (Node n : l){

			if (priority(n)<priority(min)) min = n;

		}
		return min;
	}

	/**return list of neighbors of Node n*/
	private LinkedList<Node> neighbors(Node n){
		LinkedList<Node> l = new LinkedList<Node>(); 
		for (Edge e : n.getExits()){
			Node other = e.getOther(n);
			l.add(other);	
		}

		return l;
	}

	/**return linear (Cartesian) distance between two nodes*/
	private double distance(Node a, Node b){

		double x = a.getX() - b.getX();
		double y = a.getY() - b.getY();

		return Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));

	}

	/**return priority p of Node n
	 * p is proportional to the nodes closeness to Earth
	 * p is inversely proportional to changes in speed (up/downgrades)
	 * p is near infinity (in practical terms) when only one life is remaining
	 * and n is hostile*/
	private double priority(Node n){

		double d = distance(n, Earth); //distance from n to Earth
		double dv = 1;
		if (n.hasSpeedUpgrade())dv+=0.2;
		if (n.isHostile()){
			dv-=0.2;
			if (lives <= 1){
				return d*100000000000000000000000000000.0;
			}
		}
		if (returnRoute.isEmpty()) return d/dv;
		if (returnRoute.contains(n)) d*=5;
		return d/dv;

	}

	/**move spacecraft along provided path*/
	private void travel(Iterable<Node> l){
		for (Node n : l){
			st.moveTo(n);
			if (n.hasSpeedUpgrade()) st.grabSpeedUpgrade();
		}
	}

	/**Returns time to traverse path, taking up/downgrades into
	 * account. Returns -1 if all three lives are lost during traversal
	 * used for testing*/
	private double timeTaken(LinkedList<Node> path){

		double t = 0; //cumulative time
		double v = 1; //speed
		int lives = 3; //remaining 'lives'

		for (int i=0; i!=path.size()-1;i++){
			if (lives == 0) return -1;
			Node a = path.get(i);
			Node b = path.get(i+1);
			double d = a.getDistance(b);
			if (a.isHostile()){lives-=1; if ( v >= 1.2) v-=0.2;}			
			if (a.hasSpeedUpgrade()){v+=0.2;}
			t += (d/v);	
		}

		return t; //(lives != 0) ? t : -1;
	}

}