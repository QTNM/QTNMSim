import matplotlib.pyplot as plt
import ROOT


def summary(fname="CrossSection.root"):
    input_file = ROOT.TFile(fname)
    tree = input_file.Get("CrossSection")

    energy = []
    sigma = []
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        energy.append(tree.Energy)
        sigma.append(tree.CrossSection)

    # Plot Geant4
    plt.plot(energy, sigma, label='Geant4')
    # Plot NIST Value - accessed 17/02/25
    nist_value = 5.662165e-3 * 2.8002852e-21
    plt.plot(18.6, nist_value, linestyle='', marker='x',
             label=f'NIST Value = {nist_value:.4E}', markersize=4)
    plt.xlabel(r'Energy (KeV)')
    plt.ylabel(r'$\sigma (m^2)$')
    plt.legend()
    plt.yscale('log')
    plt.xlim(left=0)
    plt.tight_layout()
    plt.savefig('ElasticCrossSection.png')

if __name__ == "__main__":
    summary()
