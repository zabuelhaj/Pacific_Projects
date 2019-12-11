#!/usr/bin/python

import numpy as np
import math as m
import random as r

# FastSLAM implementation within def fastSLAM().
# Author: Zachariah Abuelhaj

# User-defined function to append to Yt list.
def addYtorYaux(Y, x, N, mu, sigma, i):
	Y[0] = x			# Pose is always first element.
	Y[1] = N			# Number of particles is always second element.
	Y.append(mu)		# These next three values always append to data struct.
	Y.append(sigma)
	Y.append(i)
	return Y


# Don't use:
# User-defined function, initialize the first set of variables.
def initYt():
	# Define x0, N0, and mu, ∑, and i.
	x0 = np.array([[10.3],[0],[0]])
	N0 = 1
	mu0 = np.array([[30],[0],[0]])
	sigma0 = np.array([[1000000, 1000000, 1000000], [1000000, 1000000, 1000000], [1000000, 1000000, 1000000]])
	i0 = 1

	# Define the initial Y0;
	# Is a simple list of numpy arrays, which are our vectors and matrices.
	Y0 = [0,0]			# Initialize the list and append to make bigger.
	Y0 = addYtorYaux(Y0, x0, N0, mu0, sigma0, i0)
	return Y0

# User-defined function, sample;
# This adds triangular sample distribution to the wheel sensor data.
def sample(b):
	# Parameter b is actually b-squared. So we will need to fix that.
	b = int(m.sqrt(b))
	return m.sqrt(6)/2 * (r.randint(-b,b) + r.randint(-b,b))

# User-defined function, sample the new pose with sample motion model algorithm;
# The book only calls this p, so I will p it too.
def p(xt_1, ut):
	v = ut[0][0]		# Velocity is the first element in ut array.
	w = ut[1][0]		# Omega is the second element in the ut array.
	t = ut[2][0]		# Motion model returns time.
	d = ut[3][0]		# Drive or turn?
	av = 0.005
	aw = 0.500

	# Get the noisy forward velocity.
	v_hat = v + sample(av*v**2 + aw*w**2)
	w_hat = w + sample(av*v**2 + aw*w**2)

	# Initialize the pose vector;
	# Get the current pose and return.
	xt = np.array([[0],[0],[0]]).astype(float)
	if (d == 0):
		xt[0] = xt_1[0][0] + v_hat*t*m.cos(xt_1[2][0])			# Forward/backward motion model.
		xt[1] = xt_1[1][0] + v_hat*t*m.sin(xt_1[2][0])
		xt[2] = xt_1[2][0] + 0
	else:
		xt[0] = xt_1[0][0]
		xt[1] = xt_1[1][0]
		xt[2] = xt_1[2][0] + w_hat*t 							# Turning motion model.
	return xt

# User-defined function that returns the measurement prediction;
# The book only calls this h, so I will go to h as well.
def h(mu, x):
	# Initialize the vector z:
	z = np.array([[0],[0],[0]]).astype(float)

	# Solve for delta, the relative position of the landmark.
	delta = np.array([[mu[0][0] - x[0][0]],[mu[1][0] - x[1][0]]])

	# Solve for variable q.
	q = np.dot(np.transpose(delta), delta)
	q = q[0][0]

	# Solve for the measurement model:
	z[0] = m.sqrt(q)
	z[1] = m.atan2(delta[1][0], delta[0][0]) - x[2][0]
	# z[2] is signature, which I was advised we don't really care about.

	print("Z Hat: ")
	print(z, "\n")

	return z

# User-defined function to calculate the Jacobian H;
# Uses Dr. Basha's awesome method from handout #1.
def calcJacobian(mu, x):

	print("mu: ")
	print(mu, "\n")

	# Solve for delta, the relative position of the landmark.
	delta = np.array([[mu[0][0] - x[0][0]],[mu[1][0] - x[1][0]]])

	print("Delta: ")
	print(delta, "\n")

	# Solve for some weird variable q, using fun matrix math!
	q = np.dot(np.transpose(delta), delta)
	q = q[0][0]			# I don't know why python is like this, but it is.

	print("q: ")
	print(q, "\n")

	# Finally solve the Jacobian;
	# To make it easier, read some things into variables;
	# Need to cast q matrix, because python.
	qdx = np.sqrt(q)*delta[0][0]
	qdy = np.sqrt(q)*delta[1][0]
	H = (1/q)*np.array([[qdx, qdy, 0],[-delta[1][0], delta[0][0], 0], [0, 0, q.astype(float)]])

	print("Jacobian: ")
	print(H, "\n")

	return H

# User-defined function to calculate the measurement covariance.
# This is used somewhat frequently throughout the algorithm.
def calcCovariance(H, sigma):

	print("Sigma: ")
	print(sigma, "\n")

	# Set the predetermined measurement noise matrix;
	# These uncertainties are based off of the standard;
	# deviation of wheel rotation and middle IR sensor range.
	Q = np.array([[0.245, 0, 0], [0, 0.899, 0], [0, 0, 1]])

	# Find the measurement covariance.
	stemp1 = np.dot(H, sigma)
	stemp2 = np.dot(stemp1, np.transpose(H))
	S = stemp2 + Q

	print("Covariance matrix: ")
	print(S, "\n")

	return S

# User-defined function, fastSLAM();
# Accepts sensed features zt, robot pose ut, and particle data structure Yt_1.
def fastSLAM(zt, ut, Yt_1):
	# Get the number of particles in Yt_1.
	M = int((len(Yt_1) - 2)/3)											# First two elements are not particles.

	# Get the number of features.
	Nt_1 = Yt_1[1]

	p0 = 0.1															# Importance weight for new features.
	wk = [0] * (M + 1)													# This holds the MAX liklihood each particle set.

	# Initialize the Identity Matrix.
	I = np.array([[1, 0, 0], [0, 1, 0], [0, 0, 1]])

	# Loop over all particles:
	for k in range(0,M+1):
		# Sample a new pose.
		xt = p(Yt_1[0], ut)

		print("Robot sample pose: ")
		print(xt, "\n")

		# Set mu count and sigma count for next for loop.
		mcount = 2
		scount = 3

		# Create list for w, likelihood correspondance.
		w = []

		# For each feature, get the measurement likelihood.
		for j in range(0,Nt_1):
			print("Feature 1")

			# Measure prediction.
			z_hat = h(Yt_1[mcount], xt)

			# Calculate the Jacobian.
			H = calcJacobian(Yt_1[mcount], xt)

			# Get the covariance.
			S = calcCovariance(H, Yt_1[scount])

			# Get the likelihood correspondance;
			# This is the importance weight of each feature.
			wtemp1 = (np.linalg.det(2*m.pi*S)**(-1/2))
			wexp = -0.5*np.transpose(zt - z_hat)
			wexp = np.dot(wexp, np.linalg.inv(S))
			wexp = np.dot(wexp, (zt - z_hat))
			wtemp2 = wtemp1*np.exp(wexp)
			w.append(wtemp2[0][0])									# Python business again.

			# At end of loop, increment counters by 3.
			mcount = mcount + 3
			scount = scount + 3

		# Before the next loop, need to do a few chores.
		w.append(p0)												# Add new feature weight.
		print(wk, k, M)
		wk[k] = max(w)												# Max likelihood correspondance.
		c = np.argmax(w) + 1
		N = max(Nt_1, c)											# Get new number of features in map.

		# Set mu count and sigma count for next for loop.
		mcount = 2
		scount = 3
		icount = 4

		# Initialize list of features to discard from Yt_1 list.
		discard = []
		Yaux = [xt, N]												# Also initialize auxillary Y list.

		# Update the Kalman filters.
		for j in range(1,N+1):
			print("First iteration updating Kalman Filters.")
			if (j == c and c == Nt_1 + 1):
				print("Adding new particle.")

				# Initialize the mean.
				mu = xt + np.array([[zt[0][0] * m.cos(zt[1][0] + xt[2][0])], [zt[1][0] * m.sin(zt[1][0] + xt[2][0])], [0]])

				# Find the Jacobian and initialize the covariance.
				H = calcJacobian(mu, xt)
				Q = np.array([[0.245, 0, 0], [0, 0.899, 0], [0, 0, 1]])
				sigma = np.dot(np.dot(np.transpose(np.linalg.inv(H)), Q), np.linalg.inv(H))
				i = 1												# Initialize the counter.
			elif (j == c and c <= Nt_1):
				print("Observed feature.")
				# Calculate the Kalman gain:
				H = calcJacobian(Yt_1[mcount], xt)
				S = calcCovariance(H, Yt_1[scount])

				# Don't know if this is wrong or right.
				K = np.dot(np.dot(Yt_1[scount], np.transpose(H)), np.linalg.inv(S))

				# Update the mean:
				z_hat = h(Yt_1[mcount], xt)
				mu = Yt_1[mcount] + np.dot(K, (zt - z_hat))
				sigma = np.dot((I - np.dot(K, H)), Yt_1[scount])

				# Increment counter.
				i = Yt_1[icount] + 1
			else:
				print("Hit the else statement.")

				# Copy the old mean and covariance.
				mu = Yt_1[mcount]
				sigma = Yt_1[scount]

				# If the mean is out of the perceptial range of the pose...
				# Lets just assume that when this condition is met, that the feature;
				# Needs to be removed from the Yt list eventually.
				i = Yt_1[icount] - 1
				discard.append(j)

			# Save the current particle data to an auxillary Yaux list.
			Yaux = addYtorYaux(Yaux, xt, N, mu, sigma, i)


			# Increment the extra loop counters.
			mcount = mcount + 3
			scount = scount + 3
			icount = icount + 3

	# Remove the bad features.
	for deletThis in discard:
		del Yaux[deletThis*3-1]				# Removes the mu value.
		del Yaux[deletThis*3]				# Removes the sigma value.
		del Yaux[deletThis*3+1]				# Removes the i counter.
		del wk[deletThis]					# Also delete the associated weight.

	# Construct new particle set, Yt, and resample particles.
	M = int((len(Yaux) - 2)/3)
	Yt = [Yaux[0], Yaux[1]]
	population = [p for p in range(1, M+1)]

	print(len(population), len(wk))
	print(population, wk)

	# Do M times:
	for k in range(0, M):
		# Draw index k with probability weight wk.
		index = r.choices(population, wk)
		index = index[0]
		Yt.append(Yaux[index*3-1])				# Set mu value.
		Yt.append(Yaux[index*3])				# Set sigma value.
		Yt.append(Yaux[index*3+1])				# Set i counter.

	return Yt










