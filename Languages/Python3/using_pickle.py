# Original source code found at : https://wiki.python.org/moin/UsingPickle

# Save a dictionary into a pickle file.
import pickle

favorite_color = { "lion": "yellow", "kitty": "red" }

pickle.dump( favorite_color, open( "save.p", "wb" ) )

# Load the dictionary back from the pickle file.
favorite_color_loaded = pickle.load( open( "save.p", "rb" ) )

# favorite_color_loaded is now { "lion": "yellow", "kitty": "red" }
print(favorite_color_loaded)

