#
# tlibs Julia module
# @author Tobias Weber <tobias.weber@tum.de>
# @date 23-apr-2017
# @license GPLv2 or GPLv3
#

__precompile__()
module tl


#
# initialises tlibs
#
function __init__()
	ccall((:load_tlibs, :tlibs_jl), Void, ())
end


#
# loads instrument data files
#
function loadinstr(strFile::String) :: Array{Any, 1}
	scandata = ccall((:load_instr, :tlibs_jl), Array{Any, 1}, (Cstring,), strFile)

	thedict = Dict{String,String}()
	for (key, val) in zip(scandata[3], scandata[4])
		thedict[key] = val
	end

	# header names, data matrix, property dict
	return [ scandata[1], scandata[2], thedict ]
end


end




# -----------------------------------------------------------------------------
# test

a = tl.loadinstr("/home/tweber/Measurements/mira-mgv2o4-17/data/11797_00025700.dat")
cols = a[1]
data = a[2]
#println(cols)
#println(typeof(data))

idxE = findfirst(cols, "E")
idxCtr = findfirst(cols, "ctr1")

Es = data[:,idxE]
cts = data[:,idxCtr]

println(lpad(cols[idxE], 8), " ", lpad(cols[idxCtr], 8))
for (E, ct) in zip(Es, cts)
	println(lpad(E, 8), " ", lpad(ct, 8))
end
# -----------------------------------------------------------------------------
