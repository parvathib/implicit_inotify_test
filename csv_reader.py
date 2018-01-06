from collections import defaultdict
import os
import numpy as np 
import matplotlib.pyplot as plt

def read_CSV(foldername, operation):
    values = defaultdict(list)
    num_files = defaultdict(int)
    num_dirs = defaultdict(int)
    r = os.listdir(foldername);
    for file in r:
        raw_values = []
        filename = file
        file_type = filename.split('.')[len(filename.split('.'))-1]
        try:
            curr_operation = filename.split('_')[1]
        except :
            continue
        if (file_type != 'csv' or curr_operation != operation):
            continue
        variation = (filename.split('.')[0]).split('_')[len(filename.split('_'))-1]
        print variation
        fd = open('inotify_metrics/' + file)
        for line in fd:
            raw_line = line.strip('\r\n').split()
            if raw_line[1] == 'files':
                num_files[variation] = raw_line[2]
            elif raw_line[1] == 'dirs':
                num_dirs[variation] = raw_line[2]
            else:  
                line = line.strip('\r\n').split(',')
                raw_values.append(line[0])
        values[variation] = raw_values
    return values, num_dirs

[open_explicit, num_dirs] = read_CSV('inotify_metrics', 'close')
[open_implicit, num_dirs] = read_CSV('inotify_metrics', 'close-impl')

spread = map(float, open_explicit['1'])
data_explicit1 = np.asarray(sorted(spread))/1000
spread = map(float, open_implicit['1'])
data_implicit1 = np.asarray(sorted(spread))/1000

spread = map(float, open_explicit['2'])
data_explicit2 = np.asarray(sorted(spread))/1000
spread = map(float, open_implicit['2'])
data_implicit2 = np.asarray(sorted(spread))/1000

spread = map(float, open_explicit['3'])
data_explicit3 = np.asarray(sorted(spread))/1000
spread = map(float, open_implicit['3'])
data_implicit3 = np.asarray(sorted(spread))/1000

spread = map(float, open_explicit['4'])
data_explicit4 = np.asarray(sorted(spread))/1000
spread = map(float, open_implicit['4'])
data_implicit4 = np.asarray(sorted(spread))/1000

spread = map(float, open_explicit['5'])
data_explicit5 = np.asarray(sorted(spread))/1000
spread = map(float, open_implicit['5'])
data_implicit5 = np.asarray(sorted(spread))/1000

data = [data_explicit1, data_implicit1, \
        data_explicit2, data_implicit2, \
        data_explicit3, data_implicit3, \
        data_explicit4, data_implicit4, \
        data_explicit5, data_implicit5]
labels = ['Explicit\n(Dir Depth: 5)\nNo. Dirs: {}'.format(num_dirs['1']), 'Implicit\n(Dir Depth = 5)\nNo. Dirs: {}'.format(num_dirs['1']), \
          'Explicit\n(Dir Depth = 10)\nNo. Dirs: {}'.format(num_dirs['2']), 'Implicit\n(Dir Depth = 10)\nNo. Dirs: {}'.format(num_dirs['2']), \
          'Explicit\n(Dir Depth = 15)\nNo. Dirs: {}'.format(num_dirs['3']), 'Implicit\n(Dir Depth = 15)\nNo. Dirs: {}'.format(num_dirs['3']), \
          'Explicit\n(Dir Depth = 20)\nNo. Dirs: {}'.format(num_dirs['4']), 'Implicit\n(Dir Depth = 20)\nNo. Dirs: {}'.format(num_dirs['4']), \
          'Explicit\n(Dir Depth = 25)\nNo. Dirs: {}'.format(num_dirs['5']), 'Implicit\n(Dir Depth = 25)\nNo. Dirs: {}'.format(num_dirs['5'])]
colors = ['lightblue', 'lightblue', \
          'red', 'red', \
          'green', 'green', \
          'yellow', 'yellow', \
          'pink', 'pink']

fig, axes = plt.subplots(nrows=1, ncols=1, figsize=(20, 14))
bplot1 = axes.boxplot(data,
                         vert=True,
                         patch_artist=True,
                         labels=labels,
			 whis=1000)
axes.set_title('Close')
axes.set_yscale('log')
axes.set_ylabel('microseconds')
for patch, color in zip(bplot1['boxes'], colors):
    patch.set_facecolor(color)
plt.show()
