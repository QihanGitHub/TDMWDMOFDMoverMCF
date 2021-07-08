# TDMWDMOFDMoverMCF
An IBM ILOG CPLEX Optimization Studio 12.8
(CPLEX) implementation in C++ language for the reference Integer Linear Programming (ILP) model.

In the reference, the authors developed an ILP model to minimize the index of multi-dimensional resources occupied by multiple types of services in the time, frequency, and space domains. To elaborate the model in details for usual understanding, the pdf file in **Documentation** folder formulates the Routing, Core and Frequency Slot Assignment (RSCA) problem for different multiple access such as Time Division Multiplexing (TDM), Wavelength Division Multiplexing (WDM) and Orthogonal Frequency Division Multiplexing (OFDM) over Multi-Core Fibers (MCFs). The source codes in **TDMWDMOFDMonMCF** folder programs the ILP model in C++ edited in Visual Stusio Code (VSCode) by employing CPLEX to search for the optimal solutions.

For more information about this work, please refer to the corresponding folder.

## Citation
If this work help you, please cite our paper *On Throughput Optimization in Software-Defined Multi-Dimensional Space Division Multiplexing Optical Networks*. The *bib* file format is following:
```
@article{X.Zhang2021,
   author = {X. Zhang and C. Feng and X. Gong and Q. Zhang and Y. Zong and W. Hou and L. Guo},
   title = {On Throughput Optimization in Software-Defined Multi-Dimensional Space Division Multiplexing Optical Networks},
   journal = {Journal of Lightwave Technology},
   volume = {39},
   number = {9},
   pages = {2635-2651},
   year = {2021},
}
```