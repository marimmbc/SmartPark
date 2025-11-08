import os
import tensorflow as tf

model_dir = os.path.expanduser('~/parquinho_inteligente/model')
os.makedirs(model_dir, exist_ok=True)

inputs = tf.keras.Input(shape=(4,), name='serving_default_input')
x = tf.keras.layers.Dense(8, activation='relu', name='hidden')(inputs)
outputs = tf.keras.layers.Dense(1, activation='sigmoid', name='pred')(x)
model = tf.keras.Model(inputs=inputs, outputs=outputs, name='smartpark_paciencia')

model.compile(optimizer='adam', loss='mse')
tf.saved_model.save(model, model_dir)

print('✅ SavedModel gerado em:', model_dir)
