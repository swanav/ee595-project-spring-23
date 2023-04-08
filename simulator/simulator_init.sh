# Check if the ns3 folder exists
if [ -d "ns-allinone-3.38" ] 
then
    echo "ns-allinone-3.38 folder exists"

    echo "Backing up the existing folder"
    # Create a tar of existing ns3 folder
    tar -cvf backup_ns-allinone-3.38.tar ns-allinone-3.38

    # Delete the existing ns3 folder
    rm -rf ns-allinone-3.38
    # Delete the existing tar file
    rm -rf ns-allinone-3.38.tar.*
else
    echo "ns-allinone-3.38 folder does not exist"
fi

# Download the NS3 source code and build it
wget https://www.nsnam.org/releases/ns-allinone-3.38.tar.bz2

# Unpack the source code
tar -xvf ns-allinone-3.38.tar.bz2

# Remove the tar file
rm -rf ns-allinone-3.38.tar.bz2

# Create symbolic link of the new file to scratch folder
ln -sf $(pwd)/congestion_simulations ./ns-allinone-3.38/ns-3.38/scratch/congestion_simulations



