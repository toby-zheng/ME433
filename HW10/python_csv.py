import csv
import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function

time_array = [] # time
data_array = [] # data

with open('sigD.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        time_array.append(float(row[0])) # leftmost column
        data_array.append(float(row[1])) # second column

dt = (time_array[-1] - time_array[0])/len(time_array) # sampling rate

Fs = 1/dt # sample rate
Ts = 1.0/Fs; # sampling interval
ts = time_array
y = data_array # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(ts,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()


# moving average w/ a data array and a parameterized sliding window width
def moving_average(file, X):
    time_array = [] # time
    data_array = [] # data

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            time_array.append(float(row[0])) # leftmost column
            data_array.append(float(row[1])) # second column

    filtered_data = []
    length = len(time_array) - X
    filtered_data = length * [0]
    
    for i in range(len(data_array) - X):
        filtered_data[i] = sum(data_array[i:i+X]) / X

    moving_average_dt = (time_array[-1] - time_array[0])/length # sampling rate
    moving_average_ts = np.arange(0, time_array[-1], moving_average_dt)
    moving_average_Fs = 1.0/moving_average_dt # sample rate
    moving_average_y = filtered_data # the data to make the fft from
    moving_average_n = len(moving_average_y) # length of the signal
    moving_average_k = np.arange(moving_average_n)
    moving_average_T = moving_average_n/moving_average_Fs
    moving_average_frq = moving_average_k/moving_average_T # two sides frequency range
    moving_average_frq = moving_average_frq[range(int(moving_average_n/2))] # one side frequency range
    moving_average_Y = np.fft.fft(moving_average_y)/moving_average_n # fft computing and normalization
    moving_average_Y = moving_average_Y[range(int(moving_average_n/2))]

    dt = (time_array[-1] - time_array[0])/len(time_array) # sampling rate

    Fs = 1/dt # sample rate
    Ts = 1.0/Fs; # sampling interval
    ts = time_array
    y = data_array # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]


    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10,6), sharex=False)

    ax1.set(xlabel = 'Time', 
            ylabel = 'Amplitude',
            title = 'Time Domain')

    ax2.set(xlabel = 'Frequency (Hz)', 
            ylabel = 'Magnitude |Y(freq)|',
            title = 'Frequency Domain')
    

    ax1.plot(ts, y, 'k', label='Raw')
    ax2.loglog(frq, abs(Y), 'k', label='Raw')

    ax1.plot(moving_average_ts, moving_average_y, 'r', label='Moving Average')
    ax2.loglog(moving_average_frq, abs(moving_average_Y), 'r', label=f"Moving Average\nSampling number: {X}")

    ax1.legend()
    ax2.legend()
    plt.show(block=False)
    plt.show()


# moving_average("sigA.csv", 500)
# moving_average("sigB.csv", 100)
# moving_average("sigC.csv", 1)
# moving_average("sigD.csv", 20)

def iir(file, A, B):

    if (A + B != 1):
        print("Error, A and B must sum to 1\n")
        return
    
    time_array = [] # time
    data_array = [] # data

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            time_array.append(float(row[0])) # leftmost column
            data_array.append(float(row[1])) # second column
    
    filtered_data = []
    length = len(time_array)
    filtered_data = length*[0]
    
    for i in range(len(data_array)):
        if (i == 0):
            filtered_data[i] = data_array[i] * B
        else:
            filtered_data[i] = filtered_data[i-1] * A + data_array[i] * B
        
    iir_dt = (time_array[-1] - time_array[0])/length
    time = np.arange(0.0, time_array[-1], iir_dt) # 10s
    iir_Fs = 1.0 / iir_dt # sample rate
    iir_Ts = 1.0 / iir_Fs; # sampling interval
    iir_ts = time
    iir_y = filtered_data # the data to make the fft from
    iir_n = len(iir_y) # length of the signal
    iir_k = np.arange(iir_n)
    iir_T = iir_n/iir_Fs
    iir_frq = iir_k/iir_T # two sides frequency range
    iir_frq = iir_frq[range(int(iir_n/2))] # one side frequency range
    iir_Y = np.fft.fft(iir_y)/iir_n # fft computing and normalization
    iir_Y = iir_Y[range(int(iir_n/2))]

    dt = (time_array[-1] - time_array[0])/len(time_array) # step time
    Fs = 1/dt # sample rate
    Ts = 1.0/Fs; # sampling interval
    ts = time_array
    y = data_array # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10,6), sharex=False)

    ax1.set(xlabel = 'Time', 
            ylabel = 'Amplitude',
            title = 'Time Domain')

    ax2.set(xlabel = 'Frequency (Hz)', 
            ylabel = 'Magnitude |Y(freq)|',
            title = 'Frequency Domain')
    

    ax1.plot(ts, y, 'k', label='Raw')
    ax2.loglog(frq, abs(Y), 'k', label='Raw')

    ax1.plot(iir_ts, iir_y, 'r', label='IIR')
    ax2.loglog(iir_frq, abs(iir_Y), 'r', label=f"IIR\nWeight A: {A}, Weight B: {B}")

    ax1.legend()
    ax2.legend()
    plt.show(block=False)
    plt.show()


# iir("sigA.csv", .99, .01)
# iir("sigB.csv", .982, .018)
# iir("sigC.csv", 0 ,1)
# iir("sigD.csv", .74, .26)

def fir(file, sampling_number, coefficients, window_type):
    time_array = [] # time
    data_array = [] # data

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            time_array.append(float(row[0])) # leftmost column
            data_array.append(float(row[1])) # second column

    filtered_data = []
    length = len(time_array) - sampling_number
    filtered_data = length * [0]
    
    for i in range(len(data_array) - sampling_number):
        for j in range(len(coefficients)):
            filtered_data[i] += coefficients[j] * data_array[i+j]

    fir_dt = (time_array[-1] - time_array[0])/length # sampling rate
    fir_ts = np.arange(0, time_array[-1], fir_dt)
    fir_Fs = 1.0/fir_dt # sample rate
    fir_Ts = 1.0/fir_Fs; # sampling interval
    fir_y = filtered_data # the data to make the fft from
    fir_n = len(fir_y) # length of the signal
    fir_k = np.arange(fir_n)
    fir_T = fir_n/fir_Fs
    fir_frq = fir_k/fir_T # two sides frequency range
    fir_frq = fir_frq[range(int(fir_n/2))] # one side frequency range
    fir_Y = np.fft.fft(fir_y)/fir_n # fft computing and normalization
    fir_Y = fir_Y[range(int(fir_n/2))]

    dt = (time_array[-1] - time_array[0])/len(time_array) # step time

    Fs = 1/dt # sample rate
    Ts = 1.0/Fs; # sampling interval
    ts = time_array
    y = data_array # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10,6), sharex=False)

    ax1.set(xlabel = 'Time',
            ylabel = 'Amplitude',
            title = 'Time Domain')
    
    ax2.set(xlabel = 'Frequency (Hz)',
            ylabel = 'Magnitude |Y(freq)|',
            title = 'Frequency Domain')
    
    ax1.plot(ts, y, 'k', label='Raw')
    ax2.loglog(frq, abs(Y), 'k', label='Raw')

    ax1.plot(fir_ts, fir_y, 'r', label='FIR')
    ax2.loglog(fir_frq, abs(fir_Y), 'r', label=f"FIR\nSampling number: {sampling_number}, Window type: {window_type}")

    ax1.legend()
    ax2.legend()

    plt.show(block=False)
    plt.show()

h = [
    0.000000000000000000,
    -0.000012911205510004,
    -0.000050803846855900,
    -0.000109961074808602,
    -0.000182488050166534,
    -0.000254920684842473,
    -0.000306694067699512,
    -0.000308742480741087,
    -0.000222552842102759,
    0.000000000000000000,
    0.000415749987825939,
    0.001088072853470527,
    0.002083390843739197,
    0.003466443286361140,
    0.005294585383949883,
    0.007611535050755617,
    0.010441113635622234,
    0.013781602891703210,
    0.017601354260617087,
    0.021836230815986051,
    0.026389338240186324,
    0.031133318189122756,
    0.035915251812573379,
    0.040563975167151448,
    0.044899367201771026,
    0.048742960810984302,
    0.051929071582906119,
    0.054315555580229581,
    0.055793307690254271,
    0.056293697935033422,
    0.055793307690254278,
    0.054315555580229581,
    0.051929071582906119,
    0.048742960810984295,
    0.044899367201771019,
    0.040563975167151461,
    0.035915251812573400,
    0.031133318189122752,
    0.026389338240186327,
    0.021836230815986065,
    0.017601354260617093,
    0.013781602891703219,
    0.010441113635622234,
    0.007611535050755623,
    0.005294585383949891,
    0.003466443286361141,
    0.002083390843739199,
    0.001088072853470528,
    0.000415749987825939,
    0.000000000000000000,
    -0.000222552842102759,
    -0.000308742480741087,
    -0.000306694067699512,
    -0.000254920684842473,
    -0.000182488050166534,
    -0.000109961074808602,
    -0.000050803846855900,
    -0.000012911205510004,
    0.000000000000000000,
]


# fir("sigA.csv", len(h), h, "Blackman")

h1 = [
    0.000000000000000000,
    -0.000000956867960835,
    -0.000003661709520521,
    -0.000007831060726718,
    -0.000013129641594243,
    -0.000019162628739500,
    -0.000025466928148269,
    -0.000031501717753100,
    -0.000036638579416263,
    -0.000040151582670349,
    -0.000041207716759898,
    -0.000038858091937524,
    -0.000032030344599907,
    -0.000019522682953576,
    0.000000000000000000,
    0.000028007541465118,
    0.000066103081322157,
    0.000116018467401785,
    0.000179606225708159,
    0.000258827894508188,
    0.000355738596668113,
    0.000472467807260576,
    0.000611196361556671,
    0.000774129840151643,
    0.000963468561107149,
    0.001181374501492976,
    0.001429935560399128,
    0.001711127660193991,
    0.002026775260411496,
    0.002378510927152190,
    0.002767734658427438,
    0.003195573710810508,
    0.003662843703673167,
    0.004170011793050297,
    0.004717162706966069,
    0.005303968417385485,
    0.005929662190687154,
    0.006593017708910039,
    0.007292333888592717,
    0.008025425943737790,
    0.008789623145575094,
    0.009581773625962232,
    0.010398256455333234,
    0.011235001102220732,
    0.012087514251877072,
    0.012950913828908060,
    0.013819969935722138,
    0.014689152287652692,
    0.015552683599495373,
    0.016404598259518569,
    0.017238805518240793,
    0.018049156322740581,
    0.018829512845067720,
    0.019573819687287358,
    0.020276175697328511,
    0.020930905300300597,
    0.021532628240088551,
    0.022076326636250584,
    0.022557408291531002,
    0.022971765235267391,
    0.023315826556819994,
    0.023586604669698842,
    0.023781734249767126,
    0.023899503207880580,
    0.023938875186411308,
    0.023899503207880580,
    0.023781734249767129,
    0.023586604669698842,
    0.023315826556819994,
    0.022971765235267391,
    0.022557408291531002,
    0.022076326636250587,
    0.021532628240088551,
    0.020930905300300600,
    0.020276175697328515,
    0.019573819687287358,
    0.018829512845067720,
    0.018049156322740584,
    0.017238805518240797,
    0.016404598259518569,
    0.015552683599495378,
    0.014689152287652697,
    0.013819969935722147,
    0.012950913828908060,
    0.012087514251877072,
    0.011235001102220735,
    0.010398256455333234,
    0.009581773625962237,
    0.008789623145575100,
    0.008025425943737790,
    0.007292333888592719,
    0.006593017708910040,
    0.005929662190687158,
    0.005303968417385490,
    0.004717162706966068,
    0.004170011793050297,
    0.003662843703673168,
    0.003195573710810510,
    0.002767734658427441,
    0.002378510927152190,
    0.002026775260411496,
    0.001711127660193992,
    0.001429935560399129,
    0.001181374501492977,
    0.000963468561107149,
    0.000774129840151643,
    0.000611196361556671,
    0.000472467807260576,
    0.000355738596668113,
    0.000258827894508189,
    0.000179606225708159,
    0.000116018467401785,
    0.000066103081322157,
    0.000028007541465118,
    0.000000000000000000,
    -0.000019522682953576,
    -0.000032030344599907,
    -0.000038858091937524,
    -0.000041207716759898,
    -0.000040151582670349,
    -0.000036638579416263,
    -0.000031501717753100,
    -0.000025466928148269,
    -0.000019162628739500,
    -0.000013129641594243,
    -0.000007831060726718,
    -0.000003661709520521,
    -0.000000956867960835,
    0.000000000000000000,
]

# fir("sigB.csv", len(h1), h1, "Blackman")

h2 = [
    -0.002615386587763361,
    0.002833335470076980,
    0.009078520933491319,
    0.015945262945001260,
    0.023224725944169605,
    0.030682347879285761,
    0.038067422071249260,
    0.045123618841125347,
    0.051599954809132563,
    0.057261690096122309,
    0.061900632131099524,
    0.065344349643958063,
    0.067463850566859176,
    0.068179350512384371,
    0.067463850566859176,
    0.065344349643958063,
    0.061900632131099524,
    0.057261690096122309,
    0.051599954809132563,
    0.045123618841125347,
    0.038067422071249260,
    0.030682347879285761,
    0.023224725944169605,
    0.015945262945001260,
    0.009078520933491319,
    0.002833335470076980,
    -0.002615386587763361,
]

# fir("sigC.csv", len(h2), h2, "Blackman")

h3 = [
    0.000000000000000000,
    -0.000002555700403127,
    -0.000021926074457075,
    -0.000030243923556461,
    0.000035230637614817,
    0.000154398948785307,
    0.000163549902779261,
    -0.000085676630485162,
    -0.000454780054634326,
    -0.000496810707809900,
    0.000106507082185820,
    0.000994753445454925,
    0.001182413495096848,
    -0.000000000000000002,
    -0.001847339354378260,
    -0.002435803480200789,
    -0.000411452068806389,
    0.003060316850131638,
    0.004537874099887834,
    0.001416530285329076,
    -0.004628809397816192,
    -0.007846669309321930,
    -0.003459498760185020,
    0.006476196268200353,
    0.012858286311113012,
    0.007245135882819785,
    -0.008451036592230924,
    -0.020431413554762988,
    -0.014070153547348582,
    0.010343589402174131,
    0.032603623010927098,
    0.027103518882358447,
    -0.011919936975860631,
    -0.056432188881854230,
    -0.058619018046622458,
    0.012966354085632173,
    0.142398914419445355,
    0.268000958482130902,
    0.319994323137334946,
    0.268000958482130902,
    0.142398914419445355,
    0.012966354085632174,
    -0.058619018046622458,
    -0.056432188881854230,
    -0.011919936975860634,
    0.027103518882358447,
    0.032603623010927105,
    0.010343589402174131,
    -0.014070153547348587,
    -0.020431413554762995,
    -0.008451036592230924,
    0.007245135882819789,
    0.012858286311113007,
    0.006476196268200357,
    -0.003459498760185021,
    -0.007846669309321928,
    -0.004628809397816194,
    0.001416530285329077,
    0.004537874099887834,
    0.003060316850131640,
    -0.000411452068806390,
    -0.002435803480200790,
    -0.001847339354378264,
    -0.000000000000000002,
    0.001182413495096850,
    0.000994753445454926,
    0.000106507082185820,
    -0.000496810707809900,
    -0.000454780054634325,
    -0.000085676630485162,
    0.000163549902779261,
    0.000154398948785306,
    0.000035230637614817,
    -0.000030243923556461,
    -0.000021926074457074,
    -0.000002555700403127,
    0.000000000000000000,
]

fir("sigD.csv", len(h3), h3, "Blackman")