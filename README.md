<h3>
2-valued cellular automaton + diffusion:
</h3>
<p>
Generative graphics using a cellular automaton algorithm and diffusion.
</p>
<p>
see result: https://archive.org/details/bicellular1_full
</p>
<p>
The cellular automaton is "2-valued", because the value of a cell is 2-dimensional, ie. it has 2 components (red & green).
</p>
<p>
The cell's future value is determined by the 2-dimensional sum of its neighbouring cells' values.
</p>
<p>
The future value of a cell is a 2-dimensional function which returns a 2-dimensional value: its parameter is the neighbour sum in red and green, and its result is the current cell's future value, in red and green. This function is referred to as "the rule".
</p>
<p>
The rule is randomly generated using harmonic wave series.
</p>
<p>
After the cellular automaton step, diffusion is applied and the whole thing is slightly zoomed in.
</p>
<h3>
Dependencies
</h3>
<ul>
<li> glew </li>
<li> glfw </li>
<li> glm </li>
</ul>

<h3>
Installation and usage
</h3>
<p>
Install dependencies, then run make
</p>
<p>
Random seed can be given as argument to the program. If it's not specified, it is generated.
</p>
<p>
Scripts:
</p>
<ul>
<li>
repeat.py: Run program repeatedly to try different random seeds, save them in a list if they're good
</li>
<li>
viewgood.py: Run program repeatedly on random seeds that are saved in the list
</li>
<li>
rendergood.py: Same as viewgood.py, but also save all frames as image files. NOTE: this can produce huge amounts of big files!
</li>
